//ヒープ構造

template<typename T,int SIZE>
class Heap{
public:
    //const static int SIZE=1024;
    int index[SIZE];
    int size;
    
    void init(){
        size=0;
    }
    
    Heap(){
        init();
    }
    
    void insert(int newindex,T* score){
        if(size>0){
            int parent,child;
            int *hindex=index;
            child=size;
            hindex[child]=newindex;//最下位に挿入
            parent=(child-1)/2;
            
            while(score[hindex[parent]]<score[hindex[child]]){
                int i=hindex[parent];
                hindex[parent]=hindex[child];
                hindex[child]=i;
                child=parent;
                parent=(parent-1)/2;
            }
            size++;
        }else{
            index[0]=newindex;
            size=1;
        }
    }
    
    int remove(T* score){
        if(size>0){
            int parent,child1,child2;
            int *hindex=index;
            int res=hindex[0];//返り値
            int hsize=size-1;
            size--;
            hindex[0]=hindex[hsize];//最上位に最下位の値を挿入
            parent=0;
            child1=1,child2=2;
            
            while(1){
                if(child1>=hsize)return res;
                if(child2>=hsize){
                    if(score[hindex[parent]]<score[hindex[child1]]){
                        int i=hindex[parent];
                        hindex[parent]=hindex[child1];
                        hindex[child1]=i;
                    }
                    return res;
                }
                if(score[hindex[parent]]<max(score[hindex[child1]],score[hindex[child2]])){
                    int i=hindex[parent];
                    if(score[hindex[child1]]<score[hindex[child2]]){
                        hindex[parent]=hindex[child2];
                        hindex[child2]=i;
                        parent=child2;
                    }else{
                        hindex[parent]=hindex[child1];
                        hindex[child1]=i;
                        parent=child1;
                    }
                    child1=(parent<<1)+1,child2=(parent<<1)+2;
                }else{
                    break;
                }
            }
            return res;
        }else{
            return -1;
        }
    }
};
