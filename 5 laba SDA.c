#include <stdio.h>
#include <stdlib.h>


typedef struct Node {
    int data;               
    struct Node *left;      
    struct Node *right;     
} Node;


Node *create_node(int value) {
    Node *new_node = (Node *)malloc(sizeof(Node));
    if (!new_node) {
        fprintf(stderr, "Ошибка выделения памяти\n");
        exit(EXIT_FAILURE);
    }
    new_node->data  = value;
    new_node->left  = NULL;
    new_node->right = NULL;
    return new_node;
}


Node *insert(Node *root, int value) {
    if (root == NULL)
        return create_node(value);          

    if (value < root->data)
        root->left  = insert(root->left,  value);  
    else if (value > root->data)
        root->right = insert(root->right, value);   
    /* дубликаты игнорируются */

    return root;
}


void preorder(Node *root) {
    if (root == NULL) return;
    printf("%d ", root->data);      
    preorder(root->left);         
    preorder(root->right);          
}


void inorder(Node *root) {
    if (root == NULL) return;
    inorder(root->left);            
    printf("%d ", root->data);      
    inorder(root->right);           
}


void postorder(Node *root) {
    if (root == NULL) return;
    postorder(root->left);          
    postorder(root->right);         
    printf("%d ", root->data);      
}


void dfs(Node *root) {
    inorder(root);   
}


#define MAX_QUEUE 256

typedef struct {
    Node *items[MAX_QUEUE];
    int   front;
    int   rear;
} Queue;

void queue_init(Queue *q)          { q->front = q->rear = 0; }
int  queue_empty(Queue *q)         { return q->front == q->rear; }
void queue_push(Queue *q, Node *n) { q->items[q->rear++ % MAX_QUEUE] = n; }
Node *queue_pop(Queue *q)          { return q->items[q->front++ % MAX_QUEUE]; }


void bfs(Node *root) {
    if (root == NULL) return;

    Queue q;
    queue_init(&q);
    queue_push(&q, root);

    while (!queue_empty(&q)) {
        Node *current = queue_pop(&q);
        printf("%d ", current->data);

        if (current->left)  queue_push(&q, current->left);
        if (current->right) queue_push(&q, current->right);
    }
}

Node *search(Node *root, int key) {
    if (root == NULL)          return NULL;  
    if (key == root->data)     return root;  
    if (key < root->data)
        return search(root->left,  key);     
    else
        return search(root->right, key);     
}

void free_tree(Node *root) {
    if (root == NULL) return;
    free_tree(root->left);
    free_tree(root->right);
    free(root);
}

void print_menu(void) {
    printf(" 1. Добавить элемент                \n");
    printf(" 2. Прямой обход      (NLR/Preorder) \n");
    printf(" 3. Центрированный    (LNR/Inorder)  \n");
    printf(" 4. Обратный обход    (LRN/Postorder)\n");
    printf(" 5. Обход в глубину   (DFS)          \n");
    printf(" 6. Обход в ширину    (BFS)          \n");
    printf(" 7. Поиск элемента                   \n");
    printf("  0. Выход                            \n");
    printf("══════════════════════════════════════\n");
    printf("Выберите пункт: ");
}

int main(void) {
    Node *root = NULL;
    int choice, value;

    while (1) {
        print_menu();
        scanf("%d", &choice);

        switch (choice) {

            case 1:
                printf("Введите значение: ");
                scanf("%d", &value);
                root = insert(root, value);
                printf("Элемент %d добавлен.\n", value);
                break;

            case 2:
                if (root == NULL) { printf("Дерево пустое.\n"); break; }
                printf("Прямой обход (NLR): ");
                preorder(root);
                printf("\n");
                break;

            case 3:
                if (root == NULL) { printf("Дерево пустое.\n"); break; }
                printf("Центрированный обход (LNR): ");
                inorder(root);
                printf("\n");
                break;

            case 4:
                if (root == NULL) { printf("Дерево пустое.\n"); break; }
                printf("Обратный обход (LRN): ");
                postorder(root);
                printf("\n");
                break;

            case 5:
                if (root == NULL) { printf("Дерево пустое.\n"); break; }
                printf("Обход в глубину (DFS): ");
                dfs(root);
                printf("\n");
                break;

            case 6:
                if (root == NULL) { printf("Дерево пустое.\n"); break; }
                printf("Обход в ширину (BFS): ");
                bfs(root);
                printf("\n");
                break;

            case 7:
                if (root == NULL) { printf("Дерево пустое.\n"); break; }
                printf("Введите ключ для поиска: ");
                scanf("%d", &value);
                if (search(root, value))
                    printf("Элемент %d НАЙДЕН.\n", value);
                else
                    printf("Элемент %d НЕ НАЙДЕН.\n", value);
                break;

            case 0:
                printf("Выход.\n");
                free_tree(root);
                return 0;

            default:
                printf("Неверный пункт. Попробуйте снова.\n");
        }
    }
}