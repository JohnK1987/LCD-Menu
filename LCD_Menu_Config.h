#define DISPCOLUMNS         16
#define DISPROWS            02

#define  CURSORICON         '>' 
#define  EMPTYICON          ' ' 
#define  RIGHTARROW         CURSORICON
#define  LEFTARROW          '<'
#define  SLIDERICON         0xFF 
#define  SLIDEREMPTY        EMPTYICON

#define NEGATIVEANSWER      "    YES <NO>    "
#define POSITIVEANSWER      "   <YES> NO     "
#define EMPTYDATA           "Waiting..."

#if DISPROWS > 02
    #define SLIDERROWOFFSET     1  
    #define YESNOROWOFFSET      1
#else
    #define SLIDERROWOFFSET     0   
    #define YESNOROWOFFSET      0
#endif