#include "LCD_Menu.h"
#include "LCD_Menu_Config.h"
//#include "mbed.h"
#define EMPTYROW            "\0"

/* ************* Menu ************* */
Menu::Menu(){}
Menu::~Menu(){}


void Menu::attach_disp_print_cb(void (*p_print_cb)(int col_offset, int rows_n, const char **p_data)){
    p_disp_print_cb = p_print_cb;
}

void Menu::attach_disp_putc_cb(void (*p_putc_cb)(int col,int row, char c)){
    p_disp_putc_cb = p_putc_cb;
}

void Menu::init(SubMenu *p_root){
    p_activeMenu    = p_root;
    bottom          = p_activeMenu->getSelSize();
    cRedraw_req = true,
    dRedraw_req  = true;
}

void Menu::move(Menu::Direction dir){
    if(dir == Up)   {direction++;}
    if(dir == Down) {direction--;} 
    if(dir == Stay) {direction = 0;} 
}

void Menu::select(){
    button = true;
}

void Menu::polling(){

    if(direction == directionL){
        direction = Stay;
    }else if(direction > directionL){
        moveUp();
    }else if (direction < directionL){
        moveDown();
    }
    directionL = direction;

    /*if(direction != Stay){ 
        if(direction == Up) moveUp();
        else if(direction == Down) moveDown();
        direction = Stay;
    };*/

    if (button != buttonL){
        p_activeSel = &p_activeMenu->selections[cursorPos];
        if((p_activeSel->selMode != Selection::None) && (modeFlag == false)){
            modeFlag = true;
        }else{
            modeFlag = false;
            if(p_activeSel->selMode == Selection::LongTxt) (p_activeSel->selValue) = 0;
            cursorPos = 0;
            cursorLine = 1;
        }
        if(p_activeSel->p_selFunc != nullptr){
            (p_activeMenu->selections[cursorPos].p_selFunc)();
        }
        if(p_activeSel->p_childMenu != nullptr){
            p_activeMenu = p_activeSel->p_childMenu;
            bottom = p_activeMenu->getSelSize();
            cursorPos = 0;
            cursorLine = 1;
        }
        button      = false;
        cRedraw_req = true,
        dRedraw_req = true;
    }
    buttonL = button;

    if(dRedraw_req) printStr();
    if(cRedraw_req) printChar();
}

void Menu::printStr(){ 
    uint8_t cursoroffset = 1;
    char *arr[DISPROWS]= {nullptr};
    const char *dispRowStr[DISPROWS];   
    p_activeSel = &p_activeMenu->selections[cursorPos];
    uint8_t mode = p_activeSel->selMode;   
    uint8_t index = 0;
    for(uint8_t rows = 0; rows <  DISPROWS ; rows++){
        index = rows + cursorPos - (cursorLine-1); 
        if (modeFlag){
            cursoroffset = 0;
            if (p_activeSel->p_selText != nullptr){
                switch(mode){
                    case Selection::ProgressSlider:
                        if(rows == 0 + SLIDERROWOFFSET) {
                            dispRowStr[rows] = p_activeSel->p_selText;
                        }else{
                            dispRowStr[rows] = EMPTYROW;
                        }
                        break;
                    case Selection::YesNo:
                        if(rows == 0 + YESNOROWOFFSET) {
                            dispRowStr[rows] = p_activeSel->p_selText; //txt;
                        }else if(rows == 1 + YESNOROWOFFSET) {
                            if((p_activeSel->selValue)){
                                dispRowStr[rows] = POSITIVEANSWER;
                            }else{
                                dispRowStr[rows] = NEGATIVEANSWER;                                   
                            }
                        }else{
                            dispRowStr[rows] = EMPTYROW;
                        }
                        break;
                    case Selection::LongTxt:{
                        int position =  ((p_activeSel->selValue) + rows) * (DISPCOLUMNS);
                        if(myStrlen(p_activeSel->p_selText) - myStrlen(p_activeSel->p_selText + position) >= 0){
                            arr[rows] = (char*)malloc(DISPCOLUMNS + 1);
                            memcpy(arr[rows],(p_activeSel->p_selText + position),DISPCOLUMNS);
                            (*(arr[rows] + DISPCOLUMNS)) = '\0';
                            dispRowStr[rows] = arr[rows];
                        }else{
                            dispRowStr[rows] = EMPTYROW;
                        }
                        break;
                    }
                    default:
                        dispRowStr[rows] = EMPTYROW;
                        break;
                }      
            }else{
                if(rows == 0) {
                    dispRowStr[rows] = EMPTYDATA ;
                }else{
                    dispRowStr[rows] = EMPTYROW;
                }
            }
        }else{
            if (index <= p_activeMenu->getSelSize() - 1) {
                dispRowStr[rows] = p_activeMenu->selections[index].p_selName;
            }else{
                dispRowStr[rows] = EMPTYROW;
            }
        }

    }
    p_disp_print_cb(cursoroffset, DISPROWS, dispRowStr); 
    if(mode == Selection::LongTxt){    
        for (int i=0; i<DISPROWS; i++) { 
            if(arr[i] != nullptr) {
                 free(arr[i]); // delocate memory
            }
        }
    }
    dRedraw_req = false;
}

void Menu::printChar(){   
    //if(p_activeMenu->selections[cursorPos].p_childMenu == NULL) printf("No child menu cp %d\n", cursorPos);
    //else printf("child menu: %s cp %d\n", p_activeMenu->selections[cursorPos].p_childMenu->menuID, cursorPos);
    char temp;
    if (!modeFlag){
        for(uint8_t rows = 0; rows < DISPROWS ; rows++){
            if(rows == cursorLine - 1){
                temp = CURSORICON;
            }else{
                temp = EMPTYICON;
            }
            p_disp_putc_cb(0,rows,temp); 
        }
    }else{
        p_activeSel = &p_activeMenu->selections[cursorPos];
        if(p_activeSel->selMode== Selection::ProgressSlider){
            temp = LEFTARROW;
            p_disp_putc_cb(0, 1 + SLIDERROWOFFSET, temp);
            temp = RIGHTARROW;
            p_disp_putc_cb(DISPCOLUMNS - 1, 1 + SLIDERROWOFFSET, temp);
            for(uint8_t col = 0 + sizeof(char); col < DISPCOLUMNS-1; col++){
                if(col <= (p_activeSel->selValue)){
                    temp = SLIDERICON ;
                }else{
                    temp = SLIDEREMPTY;
                }
                p_disp_putc_cb(col, 1 + SLIDERROWOFFSET, temp);
            }               
        }
    }
    cRedraw_req = false;
}

void Menu::moveUp(){
    if(modeFlag == false){
        if(cursorLine > 1){
            cursorLine--;
            cRedraw_req = true;
        } 
        if(cursorPos > 0){
            cursorPos--;
            cRedraw_req = true;
            if(cursorLine ==  1 && cursorPos < bottom - DISPROWS) dRedraw_req  = true;
        }
    }else{
        p_activeSel = &p_activeMenu->selections[cursorPos];
        switch(p_activeSel->selMode){
             case Selection::YesNo:
                if(p_activeSel->selValue){
                    (p_activeSel->selValue) = false;
                    dRedraw_req = true;
                }
                break;
            case Selection::LongTxt:
                if((p_activeSel->selValue) > 0){
                    (p_activeSel->selValue)--;
                    dRedraw_req = true;
                }
                break;
            case Selection::ProgressSlider:
                if((p_activeSel->selValue) != 0){
                    (p_activeSel->selValue)--;
                    cRedraw_req = true;
                }  
                break;
            default:
                break;
        }     
    }      
}

void Menu::moveDown(){
    uint8_t rowMax = DISPROWS;
    if(modeFlag == false){
        if(bottom < DISPROWS) rowMax = bottom;
        if(cursorLine < rowMax){
            cursorLine++;
            cRedraw_req = true;
        }
        if (cursorPos != bottom-1){
            cursorPos++; 
            cRedraw_req = true;
            if(cursorLine ==  DISPROWS && cursorPos >= DISPROWS) dRedraw_req = true;
        } 
    }else{
        p_activeSel = &p_activeMenu->selections[cursorPos];
        switch(p_activeSel->selMode){
            case Selection::YesNo:
                if(!(p_activeSel->selValue)){
                    (p_activeSel->selValue) = true;
                    dRedraw_req = true;
                } 
                break;
            case Selection::LongTxt:{
                int n = myStrlen(p_activeSel->p_selText)/DISPCOLUMNS - DISPROWS + 1;
                if(n < 0) n = 0;
                if((p_activeSel->selValue) < n){
                    (p_activeSel->selValue)++;
                    dRedraw_req = true;
                }
                break;
            }
            case Selection::ProgressSlider:
                if((p_activeSel->selValue) != DISPCOLUMNS -1 - sizeof(char)){
                    (p_activeSel->selValue)++;
                    cRedraw_req = true;
                } 
                break;
            default:
                break;
        }     
    }
}

int Menu::myStrlen(const char *str){
    int i, len = 0;
    for (i=0; str[i] != 0; i++) len++;
    return(len);
}

int Menu::getDigits(int number,int base){
    int digit = 0;
    while (number) {
        digit++;
        number  /= base;
    }
    return digit;
}

const char* Menu::myItoa(int number, char* resultArr, int digits, int base){
    int i = 0;
    bool isNegative = false;
 
    if (number == 0) return "0\0";

    if (number < 0 && base == 10){
        isNegative = true;
        digits++;
        number = -number;
    }
    char tempArr[digits];
 
    while (number != 0){
        int rem = number % base;
        tempArr[i++] = (rem > 9)? (rem-10) + 'a' : rem + '0';
        number /= base;
    }

    if (isNegative) tempArr[i++] = '-';
    tempArr[i] = '\0';

    int strSize = i-1;
    for(int ii = 0; ii < i-1; ii++){
        resultArr[ii] = tempArr[strSize--];
    }
    return resultArr;
}
 

/* ****************** SubMenu ***************** */
SubMenu::SubMenu(){}
SubMenu::~SubMenu(){}

void SubMenu::add(const Selection &selection){
    selections.push_back(selection);
}

int SubMenu::getSelSize(){
    return selections.size();
}

/* *************** Selection *************** */
Selection::Selection(const char *name, void (*func)(), SubMenu *child  =  nullptr):p_selName(name),p_selFunc(func),p_childMenu(child){selMode = None;}
Selection::Selection(const char *name, void (*func)(), const char *value, Mode mode):p_selName(name), p_selFunc(func), p_selText(value), selMode(mode){}

Selection::~Selection(){}

int Selection::getValue(){
    return selValue;
}