struct snake_segment {
    int x; 
    int y;
    snake_body next;
};

typedef struct snake_segment snake_body;


struct snake {
    snake_body head; 
    int length;
};

typedef struct snake snake;