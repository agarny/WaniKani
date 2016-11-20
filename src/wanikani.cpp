/*******************************************************************************

Copyright Alan Garny

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

*******************************************************************************/

//==============================================================================
// WaniKani
//==============================================================================

#include "settings.h"
#include "wanikani.h"

//==============================================================================

#include <QDir>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QPainter>
#include <QStandardPaths>
#include <QTimer>

//==============================================================================

#include <QtSingleApplication>

//==============================================================================

#if defined(Q_OS_WIN)
    #include <Windows.h>
#elif defined(Q_OS_MAC)
    #include "macos.h"
#else
    #include <QProcess>
#endif

//==============================================================================

#include <math.h>

//==============================================================================

WaniKani::WaniKani(int pArgC, char *pArgV[]) :
    mSettings(0),
    mKanjisError(false),
    mKanjiState(QMap<QString, QString>()),
    mOldKanjiState(QMap<QString, QString>())

{
    // Create our application

    mApplication = new QtSingleApplication(QFileInfo(pArgV[0]).baseName(), pArgC, pArgV);
}

//==============================================================================

WaniKani::~WaniKani()
{
    // Delete some internal objects

    delete mSettings;
    delete mApplication;
}

//==============================================================================

int WaniKani::exec()
{
    // Check whether our application is already running

    if (mApplication->isRunning())
        return 0;

    // Customise our application

    mApplication->setApplicationName("WaniKani");
    mApplication->setOrganizationName("Hellix");
    mApplication->setQuitOnLastWindowClosed(false);

    // Create and (initially) hide our settings

    mSettings = new Settings(this);

    mSettings->hide();

    // Create a timer to generate and set our wallpaper

    mTimer = new QTimer(this);

    connect(mTimer, SIGNAL(timeout()),
            this, SLOT(updateKanjis()));

    updateInterval(mSettings->interval());

    QTimer::singleShot(0, this, SLOT(updateKanjis()));

    return mApplication->exec();
}

//==============================================================================

void WaniKani::updateInterval(const int &pInterval)
{
    // Update our timer's interval

    mTimer->start(60000*pInterval);
}

//==============================================================================

static const QString Kanjis =
"一二三四五六七八九十口日月田目古吾冒朋明唱晶品呂昌早旭世胃旦胆亘凹凸旧自白百中千舌升昇丸寸専博"
"占上下卓朝只貝貞員見児元頁頑凡負万句肌旬勺的首乙乱直具真工左右有賄貢項刀刃切召昭則副別丁町可頂"
"子孔了女好如母貫兄克小少大多夕汐外名石肖硝砕砂削光太器臭妙省厚奇川州順水氷永泉原願泳沼沖江汁潮"
"源活消況河泊湖測土吐圧埼垣圭封涯寺時均火炎煩淡灯畑災灰点照魚漁里黒墨鯉量厘埋同洞胴向尚字守完宣"
"宵安宴寄富貯木林森桂柏枠梢棚杏桐植枯朴村相机本札暦案燥未末沫味妹朱株若草苦寛薄葉模漠墓暮膜苗兆"
"桃眺犬状黙然荻狩猫牛特告先洗介界茶合塔王玉宝珠現狂皇呈全栓理主注柱金銑鉢銅釣針銘鎮道導辻迅造迫"
"逃辺巡車連軌輸前各格略客額夏処条落冗軍輝運冠夢坑高享塾熟亭京涼景鯨舎周週士吉壮荘売学覚栄書津牧"
"攻敗枚故敬言警計獄訂討訓詔詰話詠詩語読調談諾諭式試弐域賊栽載茂成城誠威滅減桟銭浅止歩渉頻肯企歴"
"武賦正証政定錠走超赴越是題堤建延誕礎婿衣裁装裏壊哀遠猿初布帆幅帽幕幌錦市姉肺帯滞刺制製転芸雨雲"
"曇雷霜冬天橋嬌立泣章競帝童瞳鐘商嫡適滴敵匕北背比昆皆混渇謁褐喝旨脂壱毎敏梅海乞乾腹複欠吹炊歌軟"
"次茨資姿諮賠培剖音暗韻識鏡境亡盲妄荒望方妨坊芳肪訪放激脱説鋭曽増贈東棟凍妊廷染燃賓歳県栃地池虫"
"蛍蛇虹蝶独蚕風己起妃改記包胞砲泡亀電竜滝豚逐遂家嫁豪腸場湯羊美洋詳鮮達羨差着唯焦礁集准進雑雌準"
"奮奪確午許歓権観羽習翌曜濯曰困固国団因姻園回壇店庫庭庁床麻磨心忘忍認忌志誌忠串患思恩応意想息憩"
"恵恐惑感憂寡忙悦恒悼悟怖慌悔憎慣愉惰慎憾憶慕添必泌手看摩我義議犠抹抱搭抄抗批招拓拍打拘捨拐摘挑"
"指持括揮推揚提損拾担拠描操接掲掛研戒械鼻刑型才財材存在乃携及吸扱丈史吏更硬又双桑隻護獲奴怒友抜"
"投没設撃殻支技枝肢茎怪軽叔督寂淑反坂板返販爪妥乳浮将奨採菜受授愛払広拡鉱弁雄台怠治始胎窓去法会"
"至室到致互棄育撤充銃硫流允唆出山拙岩炭岐峠崩密蜜嵐崎入込分貧頒公松翁訟谷浴容溶欲裕鉛沿賞党堂常"
"裳掌皮波婆披破被残殉殊殖列裂烈死葬瞬耳取趣最撮恥職聖敢聴懐慢漫買置罰寧濁環還夫扶渓規替賛潜失鉄"
"迭臣姫蔵臓賢堅臨覧巨拒力男労募劣功勧努励加賀架脇脅協行律復得従徒待往征径彼役徳徹徴懲微街衡稿稼"
"程税稚和移秒秋愁私秩秘称利梨穫穂稲香季委秀透誘穀菌米粉粘粒粧迷粋糧菊奥数楼類漆様求球救竹笑笠笹"
"筋箱筆筒等算答策簿築人佐但住位仲体悠件仕他伏伝仏休仮伯俗信佳依例個健側侍停値倣倒偵僧億儀償仙催"
"仁侮使便倍優伐宿傷保褒傑付符府任賃代袋貸化花貨傾何荷俊傍久畝囚内丙柄肉腐座卒傘匁以似併瓦瓶宮営"
"善年夜液塚幣弊喚換融施旋遊旅勿物易賜尿尼尻泥塀履屋握屈掘堀居据層局遅漏刷尺尽沢訳択昼戸肩房扇炉"
"戻涙雇顧啓示礼祥祝福祉社視奈尉慰款禁襟宗崇祭察擦由抽油袖宙届笛軸甲押岬挿申伸神捜果菓課裸斤析所"
"祈近折哲逝誓暫漸断質斥訴昨詐作雪録尋急穏侵浸寝婦掃当争浄事唐糖康逮伊君群耐需儒端両満画歯曲曹遭"
"漕槽斗料科図用庸備昔錯借惜措散廿庶遮席度渡奔噴墳憤焼暁半伴畔判券巻圏勝藤謄片版之乏芝不否杯矢矯"
"族知智矛柔務霧班帰弓引弔弘強弱沸費第弟巧号朽誇汚与写身射謝老考孝教拷者煮著署暑諸猪渚賭峡狭挟追"
"師帥官棺管父交効較校足促距路露跳躍践踏骨滑髄禍渦過阪阿際障随陪陽陳防附院陣隊墜降階陛隣隔隠堕陥"
"穴空控突究窒窃窪搾窯窮探深丘岳兵浜糸織繕縮繁縦線締維羅練緒続絵統絞給絡結終級紀紅納紡紛紹経紳約"
"細累索総綿絹繰継緑縁網緊紫縛縄幼後幽幾機玄畜蓄弦擁滋慈磁系係孫懸却脚卸御服命令零齢冷領鈴勇通踊"
"疑擬凝範犯厄危宛腕苑怨柳卵留貿印興酉酒酌酵酷酬酪酢酔配酸猶尊豆頭短豊鼓喜樹皿血盆盟盗温監濫鑑猛"
"盛塩銀恨根即爵節退限眼良朗浪娘食飯飲飢餓飾館養飽既概慨平呼坪評刈希凶胸離殺純鈍辛辞梓宰壁避新薪"
"親幸執報叫糾収卑碑陸睦勢熱菱陵亥核刻該劾述術寒醸譲壌嬢毒素麦青精請情晴清静責績積債漬表俵潔契喫"
"害轄割憲生星姓性牲産隆峰縫拝寿鋳籍春椿泰奏実奉俸棒謹勤漢嘆難華垂睡錘乗剰今含吟念琴陰予序預野兼"
"嫌鎌謙廉西価要腰票漂標栗遷覆煙南楠献門問閲閥間簡開閉閣閑聞潤欄闘倉創非俳排悲罪輩扉侯候決快偉違"
"緯衛韓干肝刊汗軒岸幹芋宇余除徐叙途斜塗束頼瀬勅疎速整剣険検倹重動勲働種衝薫病痴痘症疾痢疲疫痛癖"
"匿匠医匹区枢殴欧抑仰迎登澄発廃僚寮療彫形影杉彩彰彦顔須膨参惨修珍診文対紋蚊斉剤済斎粛塁楽薬率渋"
"摂央英映赤赦変跡蛮恋湾黄横把色絶艶肥甘紺某謀媒欺棋旗期碁基甚勘堪貴遺遣舞無組粗租祖阻査助宜畳並"
"普譜湿顕繊霊業撲僕共供異翼洪港暴爆恭選殿井囲耕亜悪円角触解再講購構溝論倫輪偏遍編冊典氏紙婚低抵"
"底民眠捕浦蒲舗補邸郭郡郊部都郵邦郷響郎廊盾循派脈衆逓段鍛后幻司伺詞飼嗣舟舶航般盤搬船艦艇瓜弧孤"
"繭益暇敷来気汽飛沈妻衰衷面革靴覇声呉娯誤蒸承函極牙芽邪雅釈番審翻藩毛耗尾宅託為偽長張帳脹髪展喪"
"巣単戦禅弾桜獣脳悩厳鎖挙誉猟鳥鳴鶴烏蔦鳩鶏島暖媛援緩属嘱偶遇愚隅逆塑岡鋼綱剛缶陶揺謡就懇墾免逸"
"晩勉象像馬駒験騎駐駆駅騒駄驚篤騰虎虜膚虚戯虞慮劇虐鹿薦慶麗熊能態寅演辰辱震振娠唇農濃送関咲鬼醜"
"魂魔魅塊襲嚇朕雰箇錬遵罷屯且藻隷癒丹潟丑卯巳謎椅翔贅芯酎俺闇枕綺鍋醤丼賂伎斐墟蜂拳遜狙噌誰呪也"
"頃叱斬鍵巾爽阜庄瞭崖箸淀堰鰐隙貼蟹鬱々";

//==============================================================================

void WaniKani::updateKanjis(const bool &pForceUpdate)
{
    // Reset some internal properties

    mKanjisError = true;
    mKanjiState = QMap<QString, QString>();

    if (pForceUpdate)
        mOldKanjiState = QMap<QString, QString>();

    // Retrieve the list of Kanjis (and their state) the user has already
    // studied

    QString url = "https://www.wanikani.com/api/v1/user/"+mSettings->apiKey()+"/kanji";
    QNetworkAccessManager networkAccessManager;

    if (!mSettings->currentKanjis()) {
        url += "/1";

        for (int i = 2; i <= 60; ++i)
            url += ","+QString::number(i);
    }

    QNetworkReply *networkReply = networkAccessManager.get(QNetworkRequest(url));
    QEventLoop eventLoop;

    QObject::connect(networkReply, SIGNAL(finished()),
                     &eventLoop, SLOT(quit()));

    eventLoop.exec();

    QByteArray response = QByteArray();

    if (networkReply->error() == QNetworkReply::NoError)
        response = networkReply->readAll();

    networkReply->deleteLater();

    if (!response.isEmpty()) {
        QJsonDocument json = QJsonDocument::fromJson(response);

        if (!json.isNull()) {
            mKanjisError = json.object().contains("error");

            QVariantMap requestedInformationMap;

            if (!mKanjisError) {
                foreach (const QVariant &requestedInformation,
                         json.object().toVariantMap()["requested_information"].toList()) {
                    requestedInformationMap = requestedInformation.toMap();

                    mKanjiState.insert(requestedInformationMap["character"].toString(),
                                       requestedInformationMap["stats"].toMap()["srs"].toString());
                }
            }
        }
    }

    // Update our wallpaper

    updateWallpaper();
}

//==============================================================================

void WaniKani::updateWallpaper(const bool &pForceUpdate)
{
    // Generate and set the wallpaper, if needed

    if (pForceUpdate || mKanjisError || (mKanjiState != mOldKanjiState)) {
        // Default wallpaper

        QPixmap pixmap;

        pixmap.load(":/wallpaper");

        if (!mKanjisError) {
            // Keep track of our Kanji/state map

            mOldKanjiState = mKanjiState;

            // Generate the wallpaper

            static const int xStart = 1240;
            static const int shift = 32;
            static const int smallShift = 1;

            int areaWidth = pixmap.width()-xStart-2*shift;
            int areaHeight = pixmap.height()-2*shift;

            QFont font = QFont(mSettings->fontName());

            font.setBold(mSettings->boldFont());
            font.setItalic(mSettings->italicsFont());

            int fontPixelSize = 1;
            int charWidth = 0;
            int charHeight = 0;
            int nbOfRows = 0;
            int nbOfCols = 0;
            int descent = 0;

            forever {
                font.setPixelSize(fontPixelSize);

                QFontMetrics fontMetrics(font);
                int crtCharWidth = fontMetrics.width(Kanjis.at(0));
                int crtCharHeight = fontMetrics.height();
                int crtNbOfCols = areaWidth/(crtCharWidth+smallShift);
                int crtNbOfRows =  floor(mKanjiState.size()/crtNbOfCols)
                                  +((mKanjiState.size() % crtNbOfCols)?1:0);

                if (crtNbOfRows*crtCharHeight+(crtNbOfRows-1)*smallShift+fontMetrics.descent() <= areaHeight) {
                    charWidth = crtCharWidth;
                    charHeight = crtCharHeight;

                    nbOfRows = crtNbOfRows;
                    nbOfCols = crtNbOfCols;

                    descent = fontMetrics.descent();

                    ++fontPixelSize;
                } else {
                    font.setPixelSize(fontPixelSize-1);

                    break;
                }
            }

            QPainter painter(&pixmap);

            painter.setFont(font);

            int x = 0;
            int y = shift+((areaHeight-nbOfRows*charHeight-(nbOfRows-1)*smallShift) >> 1)-descent;
            int radius = ceil(0.75*(qMax(charWidth, charHeight) >> 3));

            for (int i = 0, j = 0, iMax = Kanjis.size(); i < iMax; ++i) {
                if (mKanjiState.keys().contains(Kanjis.at(i))) {
                    if (!(j % nbOfCols)) {
                        x = xStart+shift;
                        y += charHeight+(j?smallShift:0);
                    }

                    QString state = mKanjiState.value(Kanjis.at(i));
                    QColor foregroundColor;
                    QColor backgroundColor;

                    if (!state.compare("apprentice")) {
                        foregroundColor = mSettings->color(2, 1);
                        backgroundColor = mSettings->color(2, 2);
                    } else if (!state.compare("guru")) {
                        foregroundColor = mSettings->color(3, 1);
                        backgroundColor = mSettings->color(3, 2);
                    } else if (!state.compare("master")) {
                        foregroundColor = mSettings->color(4, 1);
                        backgroundColor = mSettings->color(4, 2);
                    } else if (!state.compare("enlighten")) {
                        foregroundColor = mSettings->color(5, 1);
                        backgroundColor = mSettings->color(5, 2);
                    } else if (!state.compare("burned")) {
                        foregroundColor = mSettings->color(6, 1);
                        backgroundColor = mSettings->color(6, 2);
                    } else {
                        foregroundColor = mSettings->color(1, 1);
                        backgroundColor = mSettings->color(1, 2);
                    }

                    painter.setPen(foregroundColor);

                    QPainterPath path;

                    path.addRoundedRect(QRectF(x, y-charHeight+descent, charWidth, charHeight),
                                        radius, radius);

                    painter.fillPath(path, QColor(backgroundColor));
                    painter.drawText(x, y, Kanjis.at(i));

                    x += charWidth+smallShift;

                    ++j;
                }
            }
        }

        // Delete our olf wallpaper and save our new one

        if (!mSettings->fileName().isEmpty())
            QFile(mSettings->fileName()).remove();

        mSettings->setFileName(QDir::toNativeSeparators(QStandardPaths::writableLocation(QStandardPaths::PicturesLocation)+QDir::separator()+QString("WaniKani%1.jpg").arg(QDateTime::currentMSecsSinceEpoch())));

        pixmap.save(mSettings->fileName());

        // Set the new wallpaper

#if defined(Q_OS_WIN)
        SystemParametersInfo(SPI_SETDESKWALLPAPER, 0,
                             (PVOID) mSettings->fileName().utf16(), SPIF_UPDATEINIFILE);
#elif defined(Q_OS_MAC)
        setMacosWallpaper(qPrintable(mSettings->fileName()));
#else
        QProcess process;

        process.start("gsettings",
                      QStringList() << "set"
                                    << "org.gnome.desktop.background"
                                    << "picture-options"
                                    << "stretched");
        process.waitForFinished();

        process.start("gsettings",
                      QStringList() << "set"
                                    << "org.gnome.desktop.background"
                                    << "picture-uri"
                                    << QUrl::fromLocalFile(mSettings->fileName()).toString());
        process.waitForFinished();
#endif
    }
}

//==============================================================================
// End of file
//==============================================================================