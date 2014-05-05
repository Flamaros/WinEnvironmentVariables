#pragma once

#include <QString>
#include <QVariant>
#include <QList>

// TODO voir si c'est simple de gerer la mise a jour des valeurs lors d'une edition des valeurs externe (depuis l'editeur de la base des registres)

class Registry
{
public:
    static const QString    keySeparator;
private:
    static const QString    realKeySeparator;   // Win32 API requiere '\', as it's not developer friendly we do a conversion from '/'

public:
    class Key;

    class Value
    {
        friend class Key;
    public:
        enum Types
        {

        };

        Value() {}

/*        bool    load(const Key& entry, const QString& name);
        bool    save(const Key& entry);*/

        void    setName(const QString& name) {mName = name;}
        QString name() const {return mName;}

        void        setValue(const QVariant& value) {mValue = value;}
        QVariant    value() const {return mValue;}              /// Please notice values of type REG_EXPAND_SZ aren't evaluated, instead use can use evaluatedValue method
        QVariant    evaluatedValue() const {mEvaluatedValue;}   /// Same as value, except when type is REG_EXPAND_SZ

        void    setType(Types);
        Types   type() const;

    private:
        /**
          * @name is a LPWSTR
          * @type is DWORD (32bits)
          * @data depends of type (but for strings it's based on wide characters)
          */
        Value(const quint8* name, quint32 nameSize, quint32 type, const quint8* data, quint32 dataSize);

        QString     mName;
        QVariant    mValue;
        QVariant    mEvaluatedValue;
    };

    /// @brief This class represent the basic container for the values stored in the Windows registry database (Represented as folders by regedit.exe)
    class Key
    {
        struct EntryPod;

    public:
        Key();
        ~Key();

        /**
          * @path   Path of the key from the root, names must have to be separated by the keySeparator ("/")
          */
        bool    create(const QString& path);
        bool    open(const QString& path);
        void    close();

    private:
        void    registerPath(const QString& path);

        EntryPod*   mData;

        QString         mFullPath;
        QList<Value>    mValues;
    };

public:
    Registry();

    static void    exportToFile(const QString& filePath, const QStringList& keys);
};
