#include <stdio.h>
#include <string.h>

typedef struct Book {
    char *human;
    char *doing;
    char *what;
    char line[35];
} Book;

void sub(Book *book) {
    for(int i = 0; i < 4; i++) {
        printf("%s\n",book[i].line);
    }

    return;
}

int main() {
    Book book[4] = {
        {"She", "plays", "piano", ""},
        {"He", "likes", "baseball", ""},
        {"I", "drink", "beer", ""},
        {"You", "eat", "bread", ""},
    };

    
    for(int i = 0; i < 4; i++) {
        strcpy(book[i].line,book[i].human);
        strcat(book[i].line," ");
        strcat(book[i].line, book[i].doing);
        strcat(book[i].line," ");
        strcat(book[i].line, book[i].what);
        strcat(book[i].line, ".");
    }

    sub(book);

    return 0;
}