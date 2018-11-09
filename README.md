# Blauweregenをベースにしたコンピュータ大貧民プログラム

UECコンピュータ大貧民大会（UECda）[1]無差別級参加を想定した大貧民プログラムbmod（仮称）です。
@YuriCat 氏の開発した、UECda-2017無差別級優勝クライアント[2][3]をベースにしています。
後日、[3]からのフォークに直します。
BlauweregenがGPL-3.0なので、bmodもGPL-3.0です。

[1]: UECコンピュータ大貧民大会 公式ホームページ http://www.tnlab.inf.uec.ac.jp/daihinmin

[2]: 次のページで公開されているソースコードから派生 http://www.tnlab.inf.uec.ac.jp/daihinmin/2018/downloads.html

[3]: GitHub上のリポジトリ https://github.com/YuriCat/FujiGokoroUECda

# 現時点でのBlauweregenとの差異

- モンテカルロ木探索における提出手選択時のバンディットアルゴリズムを、UCB-rootからThompson Sampling（報酬値はベータ分布に従うと仮定）に変更
