/*
 heuristics.hpp
 Katsuki Ohto
 */

#ifndef UECDA_FUJI_HEURISTICS_HPP_
#define UECDA_FUJI_HEURISTICS_HPP_

#include "../include.h"

// 機械学習の対象とする部分以外のヒューリスティクス

namespace UECda{
    namespace Fuji{
        namespace Heuristics{
            Cards pruneCards(const Cards argCards, int changeQty){
                Cards tmp = argCards;
                // 絶対に渡さないカードを抜く
                // 8, 2, Joker, D3
                maskCards(&tmp , CARDS_8 | CARDS_2 | CARDS_JOKER | CARDS_D3 );
                // ジョーカーを持っているならS3
                if(containsJOKER(argCards)){
                    maskCards(&tmp, CARDS_S3);
                }
                if((int)countCards(tmp) < changeQty){
                    // 候補が足りなくなった
                    // 富豪の時、除外札のみで構成されていた場合のみ
                    // S3がいらない。あとので必勝確定
                    assert(changeQty == 1);
                    return CARDS_S3;
                    
                    // めんどいので元のまま返す
                    //return argCards;
                }
                return tmp;
            }
            
            template<class field_t>
            int preferRev(const field_t& field){
                
                uint32_t rivals = field.getRivalPlayersFlag();
                int allClassSum = 0;
                int rivalClassSum = 0;
                int numRivals = 0;
                
                for(int p = 0; p < N_PLAYERS; ++p){
                    if(field.isAlive(p) && (int)field.getMyPlayerNum() != p){
                        int cl = field.getPlayerClass(p);
                        allClassSum += cl;
                        if(rivals & (1U << p)){
                            rivalClassSum += cl;
                            ++numRivals;
                        }
                    }
                }
                if(numRivals > 0){ // ライバルプレーヤーがもういないときはどうでも良い
                    // 革命優先かの判断
                    int rivalBetter = (rivalClassSum * field.getNAlivePlayers()
                                       < allClassSum * numRivals) ? 1 : 0;
                    
                    // オーダー通常 & RPが良い階級　またはその逆の時革命優先
                    return (rivalBetter ^ field.getBoard().tmpOrder()) ? 1 : -1;
                }
                return 0;
            }
            
            template<class playSpace_t, class sbjField_t, class dice_t>
            Move chooseMateMove(playSpace_t *const ps, int NCands,
                                const sbjField_t& field,
                                const FieldAddInfo& fieldInfo, dice_t *const pdice){
                
                // 必勝着手の中から、どれを選ぶか決める
                if(NCands <= 1){
                    assert(NCands > 0);
                    return ps->getMoveById(0).mv();
                }
                
                //const FieldAddInfo fieldInfo = ps->getFInfo();
                
                // 方針は以下の通り
                // 戦略的なもの
                // L2のとき
                // 0.L2Mateでないものを優先
                
                // 1.ライバルプレーヤー(総合順位で自分が1位の場合は2位、そうでない場合は1位のプレーヤー)が上がってない場合、
                //   残りプレーヤーの相対的な順位で半分より上なら革命する、または残り手札に革命が残るようにする
                //   すでに革命されている場合は逆(DEFEAT_RIVAL_MATE設定時)
                // 見た目的なもの
                // 1.PW優先(PWフラグが付くならば...)
                // 2.独断場のとき...
                //   最小分割数が減るのであれば、パスでないものを優先
                //   そうでなければ、パスを優先
                // 3.枚数の多いものを優先
                // 4.即切り役を優先
                // 5.自分を支配していないものを優先
                
                // まず必勝着手をまとめる
                for(int m = NCands - 1; m >= 0; --m){ // 逆向きループ
                    const MoveInfo mi = ps->getMoveById(m);
                    
                    if(!mi.isMate()){
                        ps->pruneById(m);
                        --NCands;
                        continue; // 必勝でないものはなし
                    }
                }
                
                assert( NCands > 0);
                
                if(NCands == 1){ return ps->getMoveById(0).mv(); } // 1つだけしか必勝着手が無い
                
                // 複数あった
                const Cards myCards = field.getMyCards();
                
#ifdef DEFEAT_RIVAL_MATE
                int revf = 0;
                int rev = 1;
                
                if(!isNoRev(myCards)){ // 革命できるかも?
                    // まずライバルプレーヤーの動向を見る
                    
                    uint32_t rps = field.getRivalPlayersFlag();
                    
                    int clSum = 0;
                    int rpcl = 0;
                    int nrp = 0;
                    
                    for(int p = 0; p < N_PLAYERS; ++p){
                        if(field.isAlive(p) && (int)field.getMyPlayerNum() != p ){
                            int cl = field.getPlayerClass(p);
                            clSum += cl;
                            if(rps & (1U << p)){
                                rpcl += cl;
                                ++nrp;
                            }
                        }
                    }
                    
                    if(nrp > 0){ // ライバルプレーヤーがもういないときはどうでも良い
                        
                        // 革命優先かの判断
                        int rpjud = (rpcl * 1024 / nrp < clSum * 1024 / (int)field.getNAlivePlayers()) ? 1 : 0;
                        int tojud = field.getBoard().tmpOrder();
                        
                        // オーダー通常 & RPが良い階級　またはその逆の時革命優先
                        rev = rpjud ^ tojud;
                        
                        // 革命をするまたは革命を残す必勝手が存在するか確かめる
                        for(int m = NCands - 1; m >= 0; --m){ // 逆向きループ
                            
                            const Move mv = ps->getMoveById(m).mv();
                            
                            if(mv.flipsPrmOrder()){ // 革命
                                revf = 2; break;
                            }
                            if(!isNoRev(maskCards(myCards, mv.cards()))){
                                revf = 1;
                            }
                        }
                    }
                }
                
                /*if( revf ){
                 if( rev ){
                 cout<<"rev yusen!";getchar();
                 }else{
                 cout<<"norev yusen!";getchar();
                 }
                 }*/
                
#endif // DEFEAT_RIVAL_MATE
                
                //ここから選択
                MoveInfo best = ps->getMoveById(0);
                
#ifdef DEFEAT_RIVAL_MATE
                int aftrev = 0;
                if(!isNoRev(maskCards(myCards, best.cards()))){
                    aftrev = 1;
                }
#endif // DEFEAT_RIVAL_MATE
                
                int cnt = 2;
                
                for(int m = NCands - 1; m >= 1; --m){ // 逆向きループ
                    const MoveInfo mi = ps->getMoveById(m);
                    
                    if(field.getNAlivePlayers() > 2){
#ifdef DEFEAT_RIVAL_MATE
                    REV:
                        if(revf){
                            if(rev){
                                if(mi.flipsPrmOrder()){
                                    if(best.flipsPrmOrder()){
                                        goto PW;
                                    }else{
                                        best = mi;
                                        continue;
                                    }
                                }else{
                                    if(best.flipsPrmOrder()){
                                        continue;
                                    }
                                }
                            }else{ // 革命しない事を優先
                                if(!mi.flipsPrmOrder() ){
                                    if(!best.flipsPrmOrder()){
                                        goto AFTREV;
                                    }else{
                                        best = mi;
                                        continue;
                                    }
                                }else{
                                    if(!best.flipsPrmOrder()){
                                        continue;
                                    }
                                }
                            }
                        }
                        
                    AFTREV:
                        
                        if(revf){
                            if(rev){
                                if(!isNoRev(maskCards(myCards, mi.cards()))){
                                    if(!aftrev){
                                        best = mi;
                                        continue;
                                    }
                                }else{
                                    if(aftrev){
                                        continue;
                                    }
                                }
                            }else{ // 革命しない事を優先
                                if(isNoRev(maskCards(myCards, mi.cards()))){
                                    if(aftrev){
                                        best = mi;
                                        continue;
                                    }
                                }else{
                                    if(!aftrev){
                                        continue;
                                    }
                                }
                            }
                        }
#endif // DEFEAT_RIVAL_MATE
                    }else{
                        if(mi.isL2Mate()){
                            if(!best.isL2Mate()){
                                continue;
                            }
                        }else{
                            if(best.isL2Mate()){
                                best = mi;
                                continue;
                            }
                        }
                    }
                PW:
                    if(mi.isPW()){
                        if(!best.isPW()){
                            best = mi;
                            continue;
                        }
                    }else{
                        if(best.isPW()){
                            continue;
                        }
                    }
                UR_MPN:
                    if(fieldInfo.isUnrivaled()){
                        if(mi.isPASS()){
                            if(best.getIncMinNMelds() > 0){
                                best = mi; continue;
                            }
                        }else{
                            if(best.isPASS()){
                                if(mi.getIncMinNMelds() == 0){
                                    best = mi; continue;
                                }
                            }else{
                                if(mi.getIncMinNMelds() < best.getIncMinNMelds()){
                                    best = mi; continue;
                                }else if(mi.getIncMinNMelds() > best.getIncMinNMelds()){
                                    continue;
                                }
                            }
                        }
                    }
                    
                QTY:
                    if(mi.qty() > best.qty()){
                        best = mi;
                        continue;
                    }else if(mi.qty() < best.qty()){
                        continue;
                    }
                    
                FAST_DOM:
                    if(mi.domInevitably()){
                        if(!best.domInevitably()){
                            best = mi;
                            continue;
                        }
                    }else{
                        if(best.domInevitably()){
                            continue;
                        }
                    }
                DOM_ME:
                    if(!mi.isDM()){
                        if(best.isDM()){
                            best = mi;
                            continue;
                        }
                    }else{
                        if(!best.isDM()){
                            continue;
                        }
                    }
                    
                    //RND:
                    //ランダム選択
                    assert(cnt > 0);
                    if(pdice->rand() % cnt == 0){
                        best = mi;
                    }
                    ++cnt;
                }
                return best.mv();
            }
            
            template<class playSpace_t, class sbjField_t>
            int pruneRoot(playSpace_t *const ps, const int NCands, const sbjField_t& field){
                // 枝刈り(ルートノード)
                
                // 場の情報から、ヒューリスティックな枝刈りを行う
                // 後回し枝刈りのような場合には後着手の拘束条件を付加することもある
                // 枝刈りされた候補はswapにより後方へ回す
                
                // ラスト2人などの限定された状況では必ず正しく、
                // それ以外でもある程度有用と考えられる枝刈りには　//論理
                // 根拠に乏しいが安定化や計算量削減のため加えている経験的な枝刈りは //経験
                // のコメントをつけている
                
                // 最後まで残った候補数を返す
                using move_t = typename playSpace_t::move_t;
                
                const FieldAddInfo& fieldInfo = ps->fieldInfo;
                const Board& bd = field.getBoard();
                //int ret = NCands;
                const Cards remCards = field.getRemCards();
                const Cards myCards = field.getMyCards();
                const Cards opsCards = field.getOpsCards();
                const Cards DWCards = getAllDWCards(myCards, opsCards, bd.tmpOrder(), fieldInfo.isTmpOrderSettled());
                
                const int NMyCards = field.getNCards(field.getMyPlayerNum());
                const int NRemCards = countCards(remCards);
                
                int tmpOrd = bd.tmpOrder();
                
                Cards hrCards;
                const Cards qr = field.getMyHand().qr;
                
                if(tmpOrd == ORDER_NORMAL){
                    hrCards = Rank4xToCards(IntCardToRank4x(pickIntCardHigh(myCards)));
                }else{
                    hrCards = Rank4xToCards(IntCardToRank4x(pickIntCardLow(myCards)));
                }
                
                //int hRank,lRank;
                
                const Cards seqCards = myCards & extractRanks<3>(field.getMyHand().seq);
                
                for(int m = NCands - 1; m >= 0; --m){ // 逆向きループ
                    
                    move_t *const mv = ps->getMovePtr(m);
                    
                    const Cards mvCards = mv->cards();
                    const Cards nextCards = subtrCards(myCards, mvCards);
                    
                    if(mv->isPASS()){ continue; }
                    
                    // パス支配の場合、次の空場で支配的ならば出さない
                    if(fieldInfo.isPassDom()){
                        if(mv->isE_NFDO()){
                            ps->pruneById(m); // 論理
                            //cout << mv << endl; getchar();
                            continue;
                        }
                        if(andCards(mvCards, hrCards)){ ps->pruneById(m); continue; }//経験
                    }
                    
                    // 後回し枝刈り
                    if(bd.isNF() && (!mv->changesPrmState())){
                        // 空場、、永続的パラメータ変更無し
                        if(hasDWorNFH(nextCards, opsCards, bd.tmpOrder(), fieldInfo.isTmpOrderSettled())){
                            // 残り手札に支配保証または空場期待または半支配保証半空場期待あり
                            // CERR<<"m:"<<OutCards(nextCards)<<" o:"<<OutCards(opsCards)<<endl;getchar();
                            mv->setDW_NFH();
                            
                            if(mv->isP_NFDO()){
                                // 永続的空場支配役
                                
                                // 枝刈りされなくても、拘束条件を付ける
                                mv->setDConst();
                                
                                // このとき単支配保証のグループは最大のもののみ残す
                                
                                if(DWCards){
                                    if(isExclusive(DWCards, mvCards)){
                                        // 当着手がそれらの係数を減らさないので即枝刈り
                                        // cerr<<mv<<endl;getchar();
                                        ps->pruneById(m); goto TO_NEXT;//論理
                                    }else{
                                        for(int mm = ps->getNActiveMoves() - 1; mm > m; --mm){
                                            move_t *const mv1=ps->getMovePtr(m);
                                            
                                            if(mv1->isP_NFDO() && (!mv1->changesPrmState()) && isExclusive (mvCards, mv1->c())){
                                                // 排反なP_NFPD役を探索することが確実な場合には枝刈り
                                                // cerr<<mv<<endl;getchar();
                                                ps->pruneById(m); goto TO_NEXT;//論理
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    
                    // ランク縮約により完全共役になりうる役の一本化
                    if(!mv->isSeq()
                       && !containsJOKER(mvCards)
                       && !andCards(mvCards, CARDS_8)
                       && mv->qty() < 4
                       && !(containsJOKER(remCards) && containsS3(mvCards))
                       ){// 階段やジョーカー使用や8切りではない
                        // スペ3含みも特別なので除外
                        // 革命は2つあることは少ないし、面倒なので無視
                        
                        int indep = (mv->qty() == ((uint32_t(qr >> mv->rank4x())) & 15U) && !andCards(mvCards, seqCards)) ? 1 : 0; // 独立性チェック
                        
                        for(int mm = ps->getNActiveMoves() - 1; mm > 0; --mm){
                            if(m == mm){ continue; }
                            move_t *const mv1 = ps->getMovePtr(m);
                            if(mv->typePart() == mv1->typePart()
                               && !mv1->containsJOKER()
                               && mv1->rank4x() != RANK4X_8
                               && mv1->qty() == ((uint32_t(qr >> mv1->rank4x())) & 15U) //こっちの独立性は必須
                               && !andCards(mv1->c(), seqCards) //こっちの独立性は必須
                               && mv->getSuitsPart() == mv1->getSuitsPart()
                               ){// 同じような性質があり、同じタイプである
                                // このランク間に自分も含め、誰もカードを持たないことを確認
                                Cards mvmvCards = addCards(mvCards, mv1->cards()); // mvとmv1のカード
                                
                                Cards rrCards = RankRange4xToCards(IntCardToRank4x(pickIntCardLow(mvmvCards)),
                                                                   IntCardToRank4x(pickIntCardHigh(mvmvCards))); // ランク間(両端含)のカード
                                
                                if(!andCards(rrCards, subtrCards(remCards, mvmvCards))){
                                    // この2役以外にこのランク間のカードがない。
                                    
                                    // 後場に出せる可能性を考え、ランクが高い方が枝刈り対象
                                    int ok = 0;
                                    if(tmpOrd == ORDER_NORMAL){
                                        if(mv->rank() > mv1->rank()){
                                            ok = 1;
                                        }
                                    }else{
                                        if(mv->rank() < mv1->rank()){
                                            ok = 1;
                                        }
                                    }
                                    if(ok){
                                        // 無条件でmvを枝刈り。
                                        //cerr<<"contraction pruning. "<<mv<<" <= "<<mv1<<endl;getchar();
                                        ps->pruneById(m); goto TO_NEXT; // 論理
                                        
                                    }
                                }else if(!indep && !andCards(rrCards, subtrCards(remCards, addCards(mvmvCards,Rank4xToCards(mv->rank4x()) & myCards)))){
                                    // mvのみ独立でないが、その2ランク間に他のカードがないことから、mvのランクが高ければmvは枝刈り。
                                    int ok = 0;
                                    if(tmpOrd == ORDER_NORMAL ){
                                        if(mv->rank() > mv1->rank()){
                                            ok = 1;
                                        }
                                    }else{
                                        if(mv->rank() < mv1->rank()){
                                            ok = 1;
                                        }
                                    }
                                    if(ok){
                                        //無条件でmvを枝刈り。
                                        //cerr<<"contraction pruning. "<<mv<<" < "<<mv1<<endl;getchar();
                                        ps->pruneById(m); goto TO_NEXT; // 論理
                                        
                                    }
                                }
                            }
                        }
                    }
                    
                    // 双子支配役（完全共役でない）に完全順序性が認められる場合、片方を外す
                    if(fieldInfo.isTmpOrderSettled()){ // オーダー固定が必須
                        if(mv->isDO()
                           && !containsJOKER(mvCards)
                           && !(containsJOKER(remCards) && containsS3(mvCards))
                           ){// 他支配あり、他諸々
                            //int indep=( mv.qty()==( (uint32_t(qr>>mv.rank4x())) & 15U) && !andCards(mvCards,seqCards))?1:0; // 独立性チェック
                            for(int mm = ps->getNActiveMoves() - 1; mm > 0; --mm){
                                if(m == mm){ continue; }
                                move_t *const mv1 = ps->getMovePtr(m);
                                
                                if(mv->typePart() == mv1->typePart()
                                   && mv->suitsPart() == mv1->suitsPart()
                                   && mv1->isDO()
                                   && !mv1->containsJOKER()
                                   && mv1->qty() == ((uint32_t(qr >> mv1->rank4x())) & 15U) //こっちの独立性は必須
                                   && !andCards(mv1->cards(), seqCards) //こっちの独立性は必須
                                   ){
                                    Cards oc0 = mvCards & ~mv1->cards();
                                    Cards oc1 = ~mvCards & mv1->cards();
                                    if( tmpOrd == ORDER_NORMAL ){
                                        if(mv->rank() > mv1->rank() && countCards(DWCards & oc0) >= countCards(DWCards&oc1)){
                                            //cout<<"priority dominance. "<<mv<<" < "<<mv1<<endl;getchar();
                                            ps->pruneById(m); goto TO_NEXT;//論理?
                                        }
                                    }else{
                                        if(mv->rank() < mv1->rank() && countCards(DWCards & oc0) >= countCards(DWCards&oc1)){
                                            //cout<<"priority dominance. "<<mv<<" < "<<mv1<<endl;getchar();
                                            ps->pruneById(m); goto TO_NEXT;//論理?
                                        }
                                    }
                                }
                            }
                        }
                    }
                    
                    //単純ジョーカー保護
                    if(mv->containsJOKER()){
                        if((!mv->changesPrmState()) && (!mv->isSeq())){
                            if(((NMyCards > 9) && (NRemCards > 20))
                               ||
                               ((NMyCards > 7) && (NRemCards > 23) && field.getNAlivePlayers() == N_PLAYERS)
                               ){
                                ps->pruneById(m);continue;//経験
                            }
                        }
                    }
                    
                    
                    if(mv->containsJOKER()){//ジョーカーありの場合の共役系の枝刈り
                        
                        for(int mm = ps->getNActiveMoves() - 1; mm > 0; --mm){
                            if(m == mm){continue;}
                            move_t *const mv1 = ps->getMovePtr(mm);
                            if(mvCards == mv1->c()){
                                //他に共役かつ完全支配なものがあれば枝刈り
                                if(mv->isDO() && mv1->isDO()){ps->pruneById(m);goto TO_NEXT;}//論理
                                //（階段で）ランクが違う場合は完全支配優先、それ以外の場合は後場オーダーで限定の強いものを優先
                                if(mv->getRank() != mv1->getRank()){
                                    if(mv1->domInevitably()){ps->pruneById(m);goto TO_NEXT;}//経験
                                    if(mv->isTmpOrderRev()){
                                        if(mv->rank() > mv1->rank()){ps->pruneById(m);goto TO_NEXT;}//経験
                                    }else{
                                        if(mv->rank() < mv1->rank()){ps->pruneById(m);goto TO_NEXT;}//経験
                                    }
                                }
                            }
                        }
                    }
                    
                    // 最小分割数を悪くするものの枝刈り
                    {
                        int r = mv->rank();
                        if(tmpOrd != ORDER_NORMAL ){ r = flipRank(r); }
                        if(mv->isSeq()){ r += mv->qty(); }
                        
                        if(mv->getIncMinNMelds() > 1 && r < RANK_J && !mv->isDO()){
                            if(!(andCards(mvCards, CARDS_3) && containsS3(myCards) && containsJOKER(remCards))){ // S3関連のみ別
                                ps->pruneById(m); goto TO_NEXT; // 経験
                            }
                        }
                        if(bd.isNF() && countCards(remCards) >= 15 &&  r < RANK_J && !mv->isDO()){
                            if(!(andCards(mvCards, CARDS_3) && containsS3(myCards) && containsJOKER(remCards))){ // S3関連のみ別
                                if(mv->getIncMinNMelds() >= 1){ ps->pruneById(m); goto TO_NEXT; } // 経験
                            }
                        }
                    }
                    
                    // ここから、拾えていないものを細かく枝狩りする予定
                    if(bd.isNF()){
                        
                        // 空場
                        if(mv->isSeq()){
                            // 空場8切りは最初の方はなし
                            if(mv->domInevitably()){
                                if(NMyCards - mv->qty() > 6){
                                    const Cards msk = tmpOrd ? RankRangeToCards(RANK_9, RANK_2) : RankRangeToCards(RANK_3, RANK_7);
                                    if(any2Cards(CardsToER(maskCards(myCards, mvCards) & msk))
                                       && (maskCards(myCards, seqCards) & msk)){
                                        ps->pruneById(m); continue; // 経験
                                    }else{
                                        //cout<<OutCards(myCards)<<endl;getchar();
                                    }
                                }
                            }
                        }else{
                            // 空場8切りは基本的になし
                            if(mv->domInevitably()){
                                // 8以下に弱い着手が2つ以上あれば、8を出す意味は少ない
                                if(NMyCards - mv->qty() > 4){
                                    const Cards msk = tmpOrd ? RankRangeToCards(RANK_9, RANK_2) : RankRangeToCards(RANK_3, RANK_7);
                                    if(any2Cards(CardsToER(maskCards(myCards, mvCards) & msk))
                                       && (maskCards(myCards, seqCards) & msk)){
                                        ps->pruneById(m); continue; // 経験
                                    }else{
                                        //cout<<OutCards(myCards)<<endl;getchar();
                                    }
                                }
                            }
                            
                            // 残り手札が無革命なときには、
                            
                            
                            
                        }
                    }
                TO_NEXT:
                    
                    if(ps->getNActiveMoves() <= 1){ break; }
                }
                return ps->getNActiveMoves();
                
            }
        }
    }
}

#endif // UECDA_FUJI_HEURISTICS_HPP_