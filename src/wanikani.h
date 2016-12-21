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

#pragma once

//==============================================================================

#include <QJsonDocument>
#include <QList>
#include <QObject>
#include <QPixmap>
#include <QString>

//==============================================================================

class StudyQueue
{
    friend class WaniKani;

public:
    explicit StudyQueue();

    int lessonsAvailable() const;
    int reviewsAvailable() const;
    int nextReviewDate() const;
    int reviewsAvailableNextHour() const;
    int reviewsAvailableNextDay() const;

private:
    int mLessonsAvailable;
    int mReviewsAvailable;
    int mNextReviewDate;
    int mReviewsAvailableNextHour;
    int mReviewsAvailableNextDay;
};

//==============================================================================

class SrsDistributionInformation
{
    friend class WaniKani;

public:
    explicit SrsDistributionInformation();

    QString name() const;
    QString radicals() const;
    QString kanji() const;
    QString vocabulary() const;
    QString total() const;

private:
    QString mName;
    QString mRadicals;
    QString mKanji;
    QString mVocabulary;
    QString mTotal;
};

//==============================================================================

class SrsDistribution
{
    friend class WaniKani;

public:
    explicit SrsDistribution();

    SrsDistributionInformation apprentice() const;
    SrsDistributionInformation guru() const;
    SrsDistributionInformation master() const;
    SrsDistributionInformation enlightened() const;
    SrsDistributionInformation burned() const;

private:
    SrsDistributionInformation mApprentice;
    SrsDistributionInformation mGuru;
    SrsDistributionInformation mMaster;
    SrsDistributionInformation mEnlightened;
    SrsDistributionInformation mBurned;
};

//==============================================================================

class Item
{
    friend class WaniKani;

public:
    explicit Item();

    QChar character() const;
    QString meaning() const;
    int level() const;

private:
    QChar mCharacter;
    QString mMeaning;
    int mLevel;
};

//==============================================================================

class UserSpecific
{
    friend class WaniKani;

public:
    explicit UserSpecific();

    QString srs() const;
    int srsNumeric() const;
    int unlockedDate() const;
    int availableDate() const;
    bool burned() const;
    int burnedDate() const;
    int meaningCorrect() const;
    int meaningIncorrect() const;
    int meaningMaxStreak() const;
    int meaningCurrentStreak() const;
    int readingCorrect() const;
    int readingIncorrect() const;
    int readingMaxStreak() const;
    int readingCurrentStreak() const;
    QString meaningNote() const;
    QString userSynonyms() const;

private:
    QString mSrs;
    int mSrsNumeric;
    int mUnlockedDate;
    int mAvailableDate;
    bool mBurned;
    int mBurnedDate;
    int mMeaningCorrect;
    int mMeaningIncorrect;
    int mMeaningMaxStreak;
    int mMeaningCurrentStreak;
    int mReadingCorrect;
    int mReadingIncorrect;
    int mReadingMaxStreak;
    int mReadingCurrentStreak;
    QString mMeaningNote;
    QString mUserSynonyms;
};

//==============================================================================

class Radical : public Item
{
    friend class WaniKani;

public:
    explicit Radical();

    QString image() const;
    UserSpecific userSpecific() const;

private:
    QString mImage;
    UserSpecific mUserSpecific;
};

//==============================================================================

typedef QList<Radical> Radicals;

//==============================================================================

class ExtraUserSpecific : public UserSpecific
{
    friend class WaniKani;

public:
    explicit ExtraUserSpecific();

    QString readingNote() const;

private:
    QString mReadingNote;
};

//==============================================================================

class Kanji : public Item
{
    friend class WaniKani;

public:
    explicit Kanji();

    QString onyomi() const;
    QString kunyomi() const;
    QString nanori() const;
    QString imporantReading() const;
    ExtraUserSpecific userSpecific() const;

private:
    QString mOnyomi;
    QString mKunyomi;
    QString mNanori;
    QString mImportantReading;
    ExtraUserSpecific mUserSpecific;
};

//==============================================================================

typedef QList<Kanji> Kanjis;

//==============================================================================

class Vocabulary : public Item
{
    friend class WaniKani;

public:
    explicit Vocabulary();

    QString kana() const;
    ExtraUserSpecific userSpecific() const;

private:
    QString mKana;
    ExtraUserSpecific mUserSpecific;
};

//==============================================================================

typedef QList<Vocabulary> Vocabularies;

//==============================================================================

class WaniKani : public QObject
{
    Q_OBJECT

public:
    explicit WaniKani();

    void setApiKey(const QString &pApiKey);

    QString userName() const;
    QString gravatar() const;
    int level() const;
    QString title() const;
    QString about() const;
    QString website() const;
    QString twitter() const;
    int topicsCount() const;
    int postsCount() const;
    int creationDate() const;
    int vacationDate() const;

    StudyQueue studyQueue() const;
    SrsDistribution srsDistribution() const;
    Radicals radicals() const;
    Kanjis kanjis() const;
    Vocabularies vocabularies() const;

private:
    QString mApiKey;

    QString mUserName;
    QString mGravatar;
    int mLevel;
    QString mTitle;
    QString mAbout;
    QString mWebsite;
    QString mTwitter;
    int mTopicsCount;
    int mPostsCount;
    int mCreationDate;
    int mVacationDate;

    StudyQueue mStudyQueue;
    SrsDistribution mSrsDistribution;
    Radicals mRadicals;
    Kanjis mKanjis;
    Vocabularies mVocabularies;

    QJsonDocument waniKaniRequest(const QString &pRequest);

    void updateSrsDistribution(const QString &pName,
                               const QVariantMap &pVariantMap,
                               SrsDistributionInformation &pSrsDistributionInformation);

signals:
    void updated();
    void error();

public slots:
    void update();
};

//==============================================================================
// End of file
//==============================================================================
