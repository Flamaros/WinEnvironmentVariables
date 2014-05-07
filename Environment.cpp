#include "Environment.h"

#include "Registry.h"

Environment::Environment()
{
    Registry::Key           path;
    QList<Registry::Value>  values;

    path.open("HKEY_LOCAL_MACHINE/SYSTEM/CurrentControlSet/Control/Session Manager/Environment");

    values = path.values();
    mSimpleVariables.reserve(values.size());
    for (int i = 0; i < values.size(); i++)
    {
        Variable    variable;

        variable.name = values[i].name();
        variable.owner = "System";
        variable.value = values[i].value().toString();
        mSimpleVariables.push_back(variable);
    }

    path.open("HKEY_CURRENT_USER/Environment");

    values = path.values();
    mSimpleVariables.reserve(mSimpleVariables.size() + values.size());
    for (int i = 0; i < values.size(); i++)
    {
        Variable    variable;

        variable.name = values[i].name();
        variable.owner = "User";
        variable.value = values[i].value().toString();
        mSimpleVariables.push_back(variable);
    }
}
