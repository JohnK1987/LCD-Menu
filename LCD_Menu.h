#ifndef LCD_MENU_H
#define LCD_MENU_H

#include <vector>

class SubMenu;
class Selection;

class Menu{
    public:
        enum Direction {
            Down    = -1,
            Stay    =  0,
            Up      =  1,
        };

        /** Class constructor */
        Menu();

        /** Class destructor */
        ~Menu();
        
        /** Attach a function to be called by the Menu for printing content to the diplay, row by row.
        * 
        * @param p_print_cb pointer to the function to be called
        * @param col_offset column offset from where the printing start, usually 0-1
        * @param rows_n     numbed of rows what need to be printed
        * @param p_data     pointer to data to be printed. The data are always end with '\0'
        *
        * @note It is need to fill this before call Menu::init()
        * @note Empty rows are filled with \0
        */
        void attach_disp_print_cb(void(*p_print_cb)(int col_offset, int rows_n, const char*p_data[]));

        /** Attach a function to be called by the Menu for put a char to specific position of display.
        * 
        * @param p_putc_cb  pointer to the function to be called
        * @param col        column where can by put a specific char or from where can start to put few chars
        * @param row        row where can by put a specific char  or from where can start to put few chars
        * @param c          pointer to char to be printed.
        *
        * @note It is need to fill this befor call Menu::init()
        */
        void attach_disp_putc_cb(void(*p_putc_cb)(int col, int row, char c));

        /** Menu ininicilization with a root menu object
        *
        * @param  p_root      pointer to root menu 
        */
        void init(SubMenu *p_root);
        
        /** The polling method is responsible for all Menu tasks like:
        *    - display re-/rendering
        *    - execution of pinned functions
        *
        * @note This method is not safe to call it from interrupt context
        */
        void polling();

        /** The select method is responsible for enter, escape current selection from menu
        * @note             this method is safe to call it from interrupt context
        */
        void select();

        /** Move function what can be call from an interupt of buttons or so on.
        *
        * @param dir        direction for move through the options of menu
        *
        * @note This method is safe to call it from interrupt context
        */
        void move(Menu::Direction dir);

    private:
        bool        button      = false,
                    buttonL     = false,
                    modeFlag    = false,
                    cRedraw_req = false,
                    dRedraw_req = false;
        int         bottom      = 0,
                    direction   = Stay,
                    directionL  = Stay,
                    cursorPos   = 0,
                    cursorLine  = 1;
        SubMenu     *p_activeMenu = nullptr;
        Selection   *p_activeSel  = nullptr; 

        void(*p_disp_print_cb)(int _col_offset, int _rows_n, const char*_data[]);
        void(*p_disp_putc_cb)(int _col, int _row, char _data);

        void        moveUp();
        void        moveDown();
        void        printStr();
        void        printChar();
        int         myStrlen(const char *str);
        int         getDigits(int number, int base);
        const char* myItoa(int number, char* resultArr, int digits, int base = 10);
};

class SubMenu{
    public:
        /** SubMenu class constructor creates Menu object which serves as storage for Selections*/
        SubMenu();

        /** SubMenu class destructor */
        ~SubMenu();
        
        /** Add one of Selection variants into the vector.
        *
        * @param selAdd         object of Selecion
        *
        * @note For more informations see Selection.h
        */
        void add(const Selection &selection);

        /** Get number of Selections of current SubMenu
        *
        * @return size of the vector selections
        *
        */
        int getSelSize();
        
    protected:
        std::vector<Selection> selections;
        
        friend class Selection; 
        friend class Menu; 
};

class Selection{
    public:
        enum Mode {
            None                = 0,
            LongTxt             = 1,
            ProgressSlider      = 2,
            YesNo               = 3
        };

        /** Selecion class constructor - default
        *
        * @param p_name       Pointer to name of Selecion what will be displayed
        * @param p_func       Pointer to to a function what may be pinned to current Selection
        * @param p_child      Pointer to another submenu - for switch to a submenu or back to previous one
        */
        Selection(const char *p_name, void (*p_func)(), SubMenu *p_child); 

        /** Selecion class constructor - with a mode string based
        *
        * @param p_name     Pointer to name of Selecion what will be displayed
        * @param p_func     Pointer to to a function what may be pinned to current Selection
        * @param p_value    Pointer to a text what may be dipslayd in the context of current mode
        * @param mode       Mode of selection - None, LongTxt, ProgressSlider, YesNo
        */
        Selection(const char *p_name, void (*p_func)(), const char *p_value, Mode mode); 

        /**Selection Class destructor */
        ~Selection();

        /** Get value accorfing to selected mode
        *
        * @return Number of slider's position or 1/0 (true/false) for question or 0 for others
        */
        int getValue();
 
    protected:
        Mode        selMode             = None;         // current mode
        SubMenu     *p_childMenu        = nullptr;      // pointer to a child submenu
        const char  *p_selName          = nullptr;      // selection name
        const char  *p_selText          = nullptr;      // pointer to string for label of question or slider, or for string of long text
        int         *p_selvalue         = nullptr;      // pointer to int value
        void        (*p_selFunc)()      = nullptr;      // pointer to a pinned function
        int         selValue            = 0;            // variable hold data according to current selection mode - slider value, 1/0 (true/false)     
             

        friend class SubMenu; 
        friend class Menu;   
};
#endif