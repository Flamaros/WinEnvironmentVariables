#pragma once

#include <QStringList>

#include "Registry.h"

// TODO Check if Windows variables are correctly updated if this application modify them throw the registry

class Environment
{
public:
    struct Variable
    {
        QString name;
        QString owner;
        QString value;
        QString expandedValue;
    };

    struct Path
    {
        QString     name;
        QString     owner;
        QStringList paths;
        QStringList evaluatedPaths;
    };

public:
    Environment();

    QList<Variable> variables() const {return mVariables;}
    QList<Path>     paths() const {return mPaths;}

private:
    void    filterVariables(const QList<Registry::Value>& values, const QString& owner);

    static bool isPath(const QString& expandedValue);

    QList<Variable> mVariables;
    QList<Path>     mPaths;
};
