#ifndef UTIL_SOCKETLISTENER_HPP_
#define UTIL_SOCKETLISTENER_HPP_

#include<iostream>
#include "../defines.h"

// 可変長ソケット通信
template<int OL = 256>
class SocketListener{
private:
    char *buf;
    int state;
public:
    SocketListener(){
        buf = new char[OL];
    }
    ~SocketListener(){
        delete[] buf;
    }
    
    void enlarge(){
        char *tbuf = new char[sizeof(buf) * 2]; // 2倍の長さにする
        memset(tbuf, 0, sizeof(tbuf));
        memcpy(tbuf, buf, l); // 情報移動
        delete[] buf;
        buf = tbuf; // バッファ付け替え
    }
    
    template<class socket_t>
    std::string recv(const socket_t& sock, const int size, const uint32_t flag){
        if(state == 0){ // バッファが空状態
            int l = recv(sock, buf, sizeof(buf), flag);
            state = l;
            while(l == sizeof(buf)){ // 満杯まで読み込んだ
                enlarge();
                l += recv(sock, buf + state, sizeof(buf) - state, flag);
                state = l;
            }
        }else{
            
        }
        
        
        while(tmp_size<size-1){
            for(int i=cur_idx;i<SOCKETLISTENER_BUF_LEN;++i){
                //1文字ずつcに移していく
                char tc=buf[i];
                if( tc=='\0' ){
                    if( tmp_size==0 ){
                        //サイズが0のとき、ヌル文字は飛ばす
                        continue;
                    }else{
                        //終了
                        if( i==SOCKETLISTENER_BUF_LEN ){
                            memset(buf,0,sizeof(buf));
                            cur_idx=-1;
                        }else{
                            ++cur_idx;
                        }
                        break;
                    }
                    c[tmp_size]=tc;
                }
            }
        }
        //最後にヌル文字を加える
        c[tmp_size]='\0';
        return tmp_size+1;
    }
};



#endif /* _SOCKETLISTENER_HPP_ */