//######################## AutomonScript Grammer #############################

grammar AMS;

//############################### LEXER ######################################

//------------------------------ KEYWORDS ------------------------------------
// PROGRAM SECTIONS
SOURCES : 'SOURCES:'; 
SOURCE  : 'SOURCE';
EVENTS  : 'EVENTS:';
EVENT   : 'EVENT';
 
OPEN    : 'OPEN';
CONSOLE : 'CONSOLE';
//----------------------------------------------------------------------------
ID      : [A-Z_][A-Z0-9_]*;
STRING  : '"' (~["\r\n])* '"';
//----------------------------------------------------------------------------
LBRACE  : '{';
RBRACE  : '}';
LPAREN  : '(';
RPAREN  : ')';
EQ      : '=';
SEMI    : ';';
//----------------------------------------------------------------------------
WS      : [ \t\r\n]+ -> skip;
//----------------------------------------------------------------------------


//############################### PARSER #####################################

//----------------------------------------------------------------------------
program 
    : sourcesBlock? eventsBlock? EOF 
    ;
//----------------------------------------------------------------------------
sourcesBlock : SOURCES sourceDefination* ;
sourceDefination : SOURCE ID LBRACE sourceItem* RBRACE ;
sourceItem    : ID ID EQ OPEN LPAREN STRING RPAREN ;
//----------------------------------------------------------------------------
eventsBlock : EVENTS eventDefination* ;
eventDefination : EVENT ID LBRACE eventItem* RBRACE ;
eventItem    : CONSOLE LPAREN STRING RPAREN SEMI ;
//----------------------------------------------------------------------------