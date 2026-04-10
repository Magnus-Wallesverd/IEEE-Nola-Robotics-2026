
void lcd_demo(void* args){
    (void) args;

    lcd_draw_rect(0, (WIDTH) - 1, 0, (HEIGHT)-1, &colors[13]);

    // background
    /*lcd_draw_rect(0, (WIDTH) - 1, 0, (HEIGHT)-1, D_GREEN);*/

    // top left 
    lcd_draw_rect(BORDER, (UI_COL_W1)-1-BORDER/2, BORDER, (UI_ROW_H1)-1-BORDER/2, &colors[12]);

    //mid left
    lcd_draw_rect(BORDER, (UI_COL_W1)-1-BORDER/2, UI_ROW_H1-1+BORDER/2, (UI_ROW_H2)-1-BORDER/2, &colors[12]);
    
    // bottom left
    lcd_draw_rect(BORDER, (UI_COL_W1)-1-BORDER/2, UI_ROW_H2-1+BORDER/2, (HEIGHT)-1-BORDER, &colors[12]);

    //top right
    lcd_draw_rect((UI_COL_W1)-1+BORDER/2, (WIDTH)-1-BORDER, BORDER, (HEIGHT/2)-1-BORDER/2, &colors[12]);
    
    // bottom right
    lcd_draw_rect((UI_COL_W1)-1+BORDER/2, (WIDTH) - 1-BORDER, HEIGHT/2-1+BORDER/2, (HEIGHT)-1-BORDER, &colors[12]);

    // middle left
    /*lcd_draw_rect(BORDER, (WIDTH/2)-1-BORDER/2, UI_ROW_H1-1+BORDER/2, UI_ROW_H2-1-BORDER/2, &colors[12]);*/

    // middle right
    /*lcd_draw_rect((WIDTH/2)-1+BORDER/2, (WIDTH)-1-BORDER, UI_ROW_H1-1+BORDER/2, UI_ROW_H2-1-BORDER/2, &colors[12]);*/

    //bottom center
    /*lcd_draw_rect(BORDER+PAD, (WIDTH)-1-BORDER-PAD, UI_ROW_H2-1+BORDER/2, HEIGHT - BORDER+1, &colors[12]);*/

}
