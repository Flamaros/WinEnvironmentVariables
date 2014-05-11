#include "Registry.h"

#include <QStringList>
#include <QDebug>

#include <windows.h>

/*
 * To dertermine best maximum size for buffer corresponding to a specific key we can use RegQueryInfoKey function.
 * This is the way to get the largest compatibility.
 */

static const quint32   maxValueNameLength = 32767;

const QString   Registry::keySeparator = "/";
const QString   Registry::realKeySeparator = "\\";

class RegistryPredefinedKeys : public QMap<QString, HKEY>
{
public:
    RegistryPredefinedKeys()
    {
        insert("HKEY_CLASSES_ROOT",     HKEY_CLASSES_ROOT);
        insert("HKEY_CURRENT_CONFIG",   HKEY_CURRENT_CONFIG);
        insert("HKEY_CURRENT_USER",     HKEY_CURRENT_USER);
        insert("HKEY_LOCAL_MACHINE",    HKEY_LOCAL_MACHINE);
        insert("HKEY_USERS",            HKEY_USERS);
    }
};

const RegistryPredefinedKeys    predefinedKeys;

//==============================================================================
//==============================================================================

Registry::Value::Value(const quint8* name, quint32 nameSize, quint32 type, const quint8* data, quint32 dataSize)
{
    // Attention pour les strings il se peux qu'elles n'aient pas de 0 finale, il faut donc se baser sur la taille
    // Les multi-string se finissent avec un double \0

    mName = QString::fromWCharArray((wchar_t*)name, nameSize);

    switch (type)
    {
    case REG_BINARY:
        qWarning("Value : \"%s\" - REG_BINARY type not yet supported", qPrintable(mName));
        break;
    case REG_DWORD: // same as REG_DWORD_LITTLE_ENDIAN
        qWarning("Value : \"%s\" - REG_DWORD type not yet supported", qPrintable(mName));
        break;
    case REG_DWORD_BIG_ENDIAN:
        qWarning("Value : \"%s\" - REG_DWORD_BIG_ENDIAN type not yet supported", qPrintable(mName));
        break;
    case REG_EXPAND_SZ:
    {
        mValue = QString::fromWCharArray((wchar_t*)data, dataSize / 2 - 1);
        mExpandedValue = expandEnvironmentStrings(mValue.toString());
        break;
    }
    case REG_LINK:
        qWarning("Value : \"%s\" - REG_LINK type not yet supported", qPrintable(mName));
        break;
    case REG_MULTI_SZ:
        qWarning("Value : \"%s\" - REG_MULTI_SZ type not yet supported", qPrintable(mName));
        break;
    case REG_NONE:
        qWarning("Value : \"%s\" - REG_NONE type not yet supported", qPrintable(mName));
        break;
    case REG_QWORD: // same as REG_QWORD_LITTLE_ENDIAN
        qWarning("Value : \"%s\" - REG_QWORD type not yet supported", qPrintable(mName));
        break;
    case REG_SZ:
    {
        mValue = QString::fromWCharArray((wchar_t*)data, dataSize / 2 - 1);
        break;
    }
    default:
        qCritical("Value : \"%s\" - Unknow type : %u", qPrintable(mName), type);
        break;
    }

    if (mExpandedValue.isNull())
        mExpandedValue = mValue;

    qDebug("Value name : \"%s\"\tvalue : \"%s\"",
           qPrintable(mName),
           qPrintable(mValue.toString()));
}

QString  Registry::Value::expandEnvironmentStrings(const QString& value)
{
    QString result;
    DWORD   length;
    LPWSTR  expandedValue;

    length = ExpandEnvironmentStringsW(value.toStdWString().c_str(), NULL, 0);  // Request the size of the destination buffer
    expandedValue = new WCHAR[length];
    length = ExpandEnvironmentStringsW(value.toStdWString().c_str(), expandedValue, length);
    result = QString::fromWCharArray((wchar_t*)expandedValue);
    delete[] expandedValue;
    return result;
}

//==============================================================================
//==============================================================================

struct Registry::Key::EntryPod
{
    HKEY    baseKey = nullptr;
    QString subKey;
    HKEY    key = nullptr;
};

Registry::Key::Key()
    : mData(new EntryPod)
{
}

Registry::Key::~Key()
{
    delete mData;
}

bool    Registry::Key::create(const QString& path)
{
    close();
    registerPath(path);
    return false;
}

bool    Registry::Key::open(const QString& path)
{
    LONG    result;

    close();
    registerPath(path);

    result = RegOpenKeyExW(mData->baseKey, mData->subKey.toStdWString().c_str(), 0, KEY_READ, &mData->key); // KEY_ALL_ACCESS
    if (result != ERROR_SUCCESS)
    {
        qCritical("Unable to open specified key : \"%s\"",
                  qPrintable(path));
        return false;
    }

    DWORD   maxValueNameLength;

    result = RegQueryInfoKeyW(mData->key,
                             NULL,
                             NULL,
                             NULL,
                             NULL,
                             NULL,
                             NULL,
                             NULL,
                             &maxValueNameLength,   // Doesn't include \0 character
                             NULL,
                             NULL,
                             NULL);
    if (result != ERROR_SUCCESS)
    {
        qCritical("Unable to query info of specified key : \"%s\"",
                  qPrintable(path));
        return false;
    }

    LPWSTR  name = nullptr;
    DWORD   nameSize = maxValueNameLength + 1;
    LPBYTE  data = nullptr;
    DWORD   dataSize = 0;
    DWORD   type;

    for (DWORD index = 0; result != ERROR_NO_MORE_ITEMS; index++)
    {
        // Don't call return in this loop, but continue insteade (doing memory release here is safer)
        delete[] name;
        name = nullptr;
        delete[] data;
        data = nullptr;

        nameSize = maxValueNameLength + 1;  // Reset to the largest value name contains in this key
        name = new WCHAR[nameSize];

        result = RegEnumValueW(mData->key,
                               index,
                               name,
                               &nameSize,
                               NULL,
                               NULL,
                               NULL,
                               &dataSize);
        if (result != ERROR_SUCCESS)
        {
            if (result != ERROR_NO_MORE_ITEMS)
                qCritical("Unable to enumerate values of specified key : \"%s\". Index : %ld",
                          qPrintable(path),
                          index);
            continue;
        }

        data = new BYTE[dataSize];

        // Now we have name we can use RegGetValueW instead of RegEnumValueW, cause it give more controls (cf. flags)
        result = RegGetValueW(mData->key,
                              NULL,
                              name,
                              RRF_RT_ANY | RRF_NOEXPAND,
                              &type,
                              data,
                              &dataSize);
        if (result == ERROR_SUCCESS)
        {
            // TODO store values
            mValues.push_back(Value((quint8*)name, nameSize, type, (quint8*)data, dataSize));
        }
        else
        {
            qWarning("Can't get data of value : \"%s\" of key : \"%s\"",
                     qPrintable(QString::fromWCharArray((wchar_t*)name, nameSize)),
                     qPrintable(mFullPath));
        }
    }
    delete[] name;
    delete[] data;

    return true;
}

void    Registry::Key::close()
{
    RegCloseKey(mData->key);

    mData->baseKey = NULL;
    mData->subKey.clear();
    mData->key = NULL ;

    mFullPath.clear();
    mValues.clear();
}

void    Registry::Key::registerPath(const QString& path)
{
    mFullPath = path;

    QStringList keys = mFullPath.split(keySeparator);

    Q_ASSERT(keys.count() > 1);
    Q_ASSERT(predefinedKeys.contains(keys[0]));

    mData->baseKey = predefinedKeys.value(keys[0]);
    mData->subKey = mFullPath;
    mData->subKey.replace(keySeparator, realKeySeparator);
    mData->subKey.remove(keys[0] + realKeySeparator);
}

//==============================================================================
//==============================================================================

Registry::Registry()
{
}

void    Registry::exportToFile(const QString& filePath, const QStringList& keys)
{
/*    QString     program = "regedit";
    QStringList arguments;

    arguments << "/E" << filePath << keys;

    QProcess*   process = new QProcess();
    process->start(program, arguments);*/

/*    LONG WINAPI RegSaveKey(
      __in      HKEY hKey,
      __in      LPCTSTR lpFile,
      __in_opt  LPSECURITY_ATTRIBUTES lpSecurityAttributes
    );*/
}
