#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fields.h"
#include "dllist.h"
#include "jrb.h"
#include <math.h>

#define MAX_SIZE 100000

#define INFINITIVE_VALUE 10000000

typedef struct _detailVertex{
    char *name;
    JRB indegreeTree;
} *detailVertex;


typedef struct _Graph{
    JRB edges;
    JRB vertices;
} *Graph;


Graph createGraph(){
    Graph g = (Graph)malloc(sizeof(struct _Graph));
    if(g==NULL) {
        printf("NULL, can not allocate mem, abort...\n");
        return NULL;
    }
    g->vertices = make_jrb();
    g->edges = make_jrb();
    return g;
}

void addVertex(Graph graph, long long int id, char* name)
{
    JRB vertices  = graph->vertices;
    JRB edges  = graph->edges;
    JRB tree;

    //update vertex
    detailVertex iver = (detailVertex)malloc(sizeof(struct _detailVertex));
    if(iver==NULL) {
        printf("NULL, can not allocate mem, abort...\n");
        return;
    }
    iver->name = strdup(name);
    iver->indegreeTree = make_jrb();
    jrb_insert_int(vertices,id,new_jval_v(iver));

    //update edge
    tree = make_jrb();
    jrb_insert_int(edges,id,new_jval_v(tree));
}

char *getVertex(Graph graph, long long int id)
{
    JRB vnode=jrb_find_int(graph->vertices,id);

    if(vnode==NULL) return NULL;
    detailVertex iver = (detailVertex) vnode->val.v;

    return iver->name;
}

void addEdge(Graph graph, long long int v1, long long int v2, double wei)
{
    JRB enode = jrb_find_int(graph->edges,v1);
    JRB vnode = jrb_find_int(graph->vertices,v2);
    JRB tree;
    if((enode==NULL)||(vnode==NULL)) {
        printf("vertex not found\n");
        return;
    }

    tree=(JRB)(enode->val).v;
    jrb_insert_int(tree,v2,new_jval_d(wei));

    detailVertex iver = (detailVertex) vnode->val.v;
    tree = iver->indegreeTree;
    jrb_insert_int(tree,v1,JNULL);
}

int hasEdge(Graph graph, long long int v1, long long int v2)
{
    JRB enode = jrb_find_int(graph->edges,v1);
    JRB tree;
    if(enode==NULL) {
        printf("vertex not found\n");
        return 0;
    }
    tree=(JRB)(enode->val).v;
    if(jrb_find_int(tree,v2)!=NULL)
        return 1;
    else return 0;
}

double getEdgeValue(Graph graph,long long int v1, long long int v2)
{
    JRB enode = jrb_find_int(graph->edges,v1);
    JRB tree;

    if(enode==NULL) {
        // printf("vertex not found\n");
        return INFINITIVE_VALUE;
    }

    tree = jrb_find_int((JRB)(enode->val).v,v2);

    if(tree==NULL){
        return INFINITIVE_VALUE;
    }

    return (tree->val).d;
}

int indegree(Graph graph,long long int v, long long int* output)
{
    JRB vnode = jrb_find_int(graph->vertices,v);
    JRB tree,node;
    int innum = 0;

    if(vnode==NULL) {
        printf("vertex not found\n");
        return 0;
    }
    detailVertex iver = (detailVertex) vnode->val.v;
    tree = iver->indegreeTree;
    //traverse tree
    jrb_traverse(node, tree){
        output[innum] = node->key.i;
        innum++;
        //printf("\nnumber innum:%d\n",output[innum-1]);
    }
    return innum;
}

int outdegree(Graph graph, long long int v,long long int* output)
{
    JRB enode = jrb_find_int(graph->edges,v);
    JRB tree,node;
    int outnum = 0;

    if(enode==NULL) {
        printf("vertex not found\n");
        return 0;
    }

    tree =(JRB)(enode->val).v;
    //traverse tree
    jrb_traverse(node, tree){
        output[outnum] = node->key.i;
        outnum++;
        //printf("\nnumber innum:%d\n",output[outnum-1]);
    }
    return outnum;
}

void dropGraph(Graph graph)
{
    JRB node;
    detailVertex detailnode;

    jrb_traverse(node,graph->edges){
        jrb_free_tree((JRB)jval_v(node->val));
    }
    jrb_free_tree(graph->edges);

    jrb_traverse(node,graph->vertices){
        detailnode = (detailVertex) node->val.v;
        free(detailnode->name);
        jrb_free_tree((JRB)detailnode->indegreeTree);
    }
    jrb_free_tree(graph->vertices);

    free(graph);
}


void readFile(Graph g, JRB sdt, char* dancupath, char* tiepxucpath, int *size, int *sotiepxuc){
    IS is = new_inputstruct(dancupath);
    if(is==0){
        perror("\nLoi file");
        return;
    }
    int num, id;
    get_line(is);
    num = atoi(is->fields[0]);
    *size = num;
    int i;
    for(i = 0; i < num; i++){
        get_line(is);
        id = atoi(is->fields[0]);
        addVertex(g, id, is->fields[2]);
        jrb_insert_int(sdt, id, new_jval_s(strdup(is->fields[1])));
    }
    jettison_inputstruct(is);
// xong file dan cu
    is = new_inputstruct(tiepxucpath);
    if(is==0){
        perror("\nLoi file");
        return;
    }
    get_line(is);
    num = atoi(is->fields[0]);
    *sotiepxuc = num;
    for(i = 0; i < num; i++){
        get_line(is);
        // printf("%s", is->text1);
        addEdge(g, atoi(is->fields[0]), atoi(is->fields[1]), atoi(is->fields[2]));
        addEdge(g, atoi(is->fields[1]), atoi(is->fields[0]), atoi(is->fields[2]));
    }
    jettison_inputstruct(is);
}

int main(void){
    long long int output[1000];
    Graph g = createGraph();
    int size, sotiepxuc;
    int i, j, n;
    long long int id;
    JRB node, tmp;
    char name[50];
    char phone[20];
    JRB sdt = make_jrb();
    int luachon;
    printf("==============MENU============|\n");
    printf("|1. Doc du lieu               |\n");
    printf("|2. Dang ki mot cong dan moi  |\n");
    printf("|3. Ghi nhan tiep xuc         |\n");
    printf("|4. Tra cuu cong dan          |\n");
    printf("|5. Tra cuu thong tin tiep xuc|\n");
    printf("|6. Tra cuu mang luoi tiep xuc|\n");
    printf("|7. Ket thuc chuong trinh     |\n");
    printf("==============END=============|\n");
    do {
        printf("\n+)Nhap lua chon cua ban: ");
        scanf("%d", &luachon);
        switch (luachon) {
            case 1:;
                readFile(g, sdt, "dancu.txt", "tiepxuc.txt", &size, &sotiepxuc);
                printf("\nTong so dinh: %d\nTong so canh: %d", size, sotiepxuc);
                break;

            case 2:;
                printf("\nNhap so CCCD moi: ");
                scanf("%lld", &id);
                printf("\nNhap ho ten: ");
                scanf("%s", name);
                printf("\nNhap sdt: ");
                scanf("%s", phone);
                if(getVertex(g, id) == NULL){
                    addVertex(g, id, name);
                    jrb_insert_int(sdt, id, new_jval_s(phone));
                    printf("\nThem cong dan");
                    size++;
                }
                else{
                    node = jrb_find_int(g->vertices,id);
                    detailVertex iver = (detailVertex) node->val.v;
                    strcpy(iver->name, name);
                    strcpy(jrb_find_int(sdt, id)->val.s, phone);
                    printf("\nCap nhap thong tin");
                }
                break;

            case 3:;
                long long int id1, id2;
                do{
                    printf("\nNhap CCCD nguoi 1: ");
                    scanf("%lld", &id1);
                    printf("\nNhap CCCD nguoi 2: ");
                    scanf("%lld", &id2);
                    if(getVertex(g, id1) == NULL || getVertex(g, id2) == NULL){
                        printf("\nKhong ton tai");
                    }
                }while(getVertex(g, id1) != NULL && getVertex(g, id2) != NULL);
                long long int time;
                printf("\nNhap thoi gian: ");
                scanf("%lld", &time);
                if(hasEdge(g, id1, id2) == 0){
                    addEdge(g, id1, id2, time);
                    printf("\nThem tiep xuc");
                    sotiepxuc++;
                }else{
                    node = jrb_find_int(g->edges,id1);
                    JRB tree;
                    tree = jrb_find_int((JRB)(node->val).v, id2);
                    tree->val.d = time;
                    printf("\nCap nhat tiep xuc");
                }
                printf("\ntong so tiep xuc: %d", sotiepxuc);
                break;

            case 4:;
                printf("\nNhap so CCCD: ");
                scanf("%lld", &id);
                if(getVertex(g, id) == NULL){
                    printf("\nKhong ton tai");
                    break;
                }
                printf("\nHo ten: %s", getVertex(g, id));
                printf("\nsdt: %s", jrb_find_int(sdt, id)->val.s);

                break;

            case 5:;

                break;

            case 6:;

                break;

            case 7:;
                printf("\nKet thuc chuong trinh");
                break;

            default:
                break;
        }
    }while(luachon != 7);

    printf("\n");
    jrb_free_tree(sdt);
    dropGraph(g);
    return 0;
}