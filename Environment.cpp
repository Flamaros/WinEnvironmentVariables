#include "Environment.h"

#include <windows.h>

Environment::Environment()
{
    Registry::Key           path;
    QList<Registry::Value>  values;

    path.open("HKEY_LOCAL_MACHINE/SYSTEM/CurrentControlSet/Control/Session Manager/Environment");
    filterVariables(path.values(), "System");

    path.open("HKEY_CURRENT_USER/Environment");
    filterVariables(path.values(), "User");
}

void    Environment::filterVariables(const QList<Registry::Value>& values, const QString& owner)
{
    mVariables.reserve(mVariables.size() + values.size());
    for (int i = 0; i < values.size(); i++)
    {
        if (isPath(values[i].expandedValue().toString()))
        {
            Path    path;

            path.name = values[i].name();
            path.owner = owner;
            path.paths = values[i].value().toString().split(";");
            path.evaluatedPaths = values[i].expandedValue().toString().split(";");
            mPaths.push_back(path);
        }
        else
        {
            Variable    variable;

            variable.name = values[i].name();
            variable.owner = owner;
            variable.value = values[i].value().toString();
            variable.expandedValue = values[i].expandedValue().toString();
            mVariables.push_back(variable);
        }
    }
}

bool    Environment::isPath(const QString& expandedValue)
{
    QRegExp reg;

    reg.setPattern("([a-zA-Z]:\\\\|\\\\\\\\?!\\\\).*");  // As Windows file system aren't case sensitive it's also the case for the drive letter
    return reg.exactMatch(expandedValue);
}
