#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <Windows.h>
#include <assert.h>
#include <direct.h>
#define GetCurrentDir _getcwd

char *strremove(char *str, const char *sub)
{
    size_t len = strlen(sub);
    if (len > 0)
    {
        char *p = str;
        while ((p = strstr(p, sub)) != NULL)
        {
            memmove(p, p + len, strlen(p + len) + 1);
        }
    }
    return str;
}

char *GetString(HKEY m_hKey, const char *name)
{
    DWORD cbData = 0;
    DWORD dwType = 0;

    DWORD dwFlags = RRF_RT_REG_EXPAND_SZ | RRF_NOEXPAND | RRF_RT_REG_SZ;

    LSTATUS lStatus = RegGetValue(m_hKey, NULL, name, dwFlags, &dwType, NULL, &cbData);
    if (lStatus != ERROR_SUCCESS)
    {
        printf("Error ocured while saving variable");
    }

    if (dwType != REG_SZ && dwType != REG_EXPAND_SZ)
    {
        printf("Error ocured while saving variable");
    }

    char *value;

    if (cbData)
    {
        assert((cbData % sizeof(TCHAR)) != 1);

        void *data = malloc(cbData / sizeof(TCHAR));
        assert(data != NULL);

        lStatus = RegGetValue(m_hKey, NULL, name, dwFlags, &dwType, data, &cbData);

        if (lStatus == ERROR_SUCCESS)
        {
            value = data;
        }
        else
        {
            printf("Error ocured while saving variable: %d \n", lStatus);
        }
    }

    return value;
}

void SetString(HKEY m_hKey, const char *name, const char *value)
{
    int cbData = strlen(value);

    LSTATUS lStatus = RegSetValueEx(m_hKey, name, 0, REG_SZ, value, cbData * sizeof(TCHAR));
    if (lStatus != ERROR_SUCCESS)
    {
        if (lStatus == 5)
        {
            printf("Permission error. Run program as administrator.");
            exit(1);
        }

        printf("Error ocured while saving variable: %d \n", lStatus);
        exit(1);
    }
}

HKEY openRegister()
{
    HKEY hKey = NULL;
    int lStatus = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "System\\CurrentControlSet\\Control\\Session Manager\\Environment", 0, KEY_READ | KEY_SET_VALUE | KEY_WOW64_64KEY, &hKey);
    if (lStatus != ERROR_SUCCESS)
    {
        if (lStatus == 5)
        {
            printf("Permission error. Run program as administrator.");
            exit(1);
        }
        printf("Error ocured while saving variable: %d \n", lStatus);
        exit(1);
    }

    return hKey;
}

int main(int argc, char *argv[])
{

    HKEY hKey = openRegister();
    char *pathToHandle;

    if (argc > 1)
    {
        pathToHandle = argv[1];
        if (*pathToHandle == '.')
        {
            char buf[100];
            GetCurrentDir(buf, 512);
            pathToHandle++;
            pathToHandle = strcat(buf, pathToHandle);
        }
    }
    else
    {
        pathToHandle = argv[0];
        char buf[100];
        GetCurrentDir(buf, 512);
        pathToHandle = buf;
    }

    char *pathEnv = GetString(hKey, "Path");
    char *position = strstr(pathEnv, pathToHandle);

    if (position)
    {
        if (*(position - 1) == ';')
        {
            char *helper = calloc(strlen(pathToHandle) + 1, sizeof(char));
            helper[0] = ';';
            helper = strcat(helper, pathToHandle);
            strremove(pathEnv, helper);
            free(helper);
        }
        else if (*(position + strlen(pathToHandle)) == ';')
        {
            pathToHandle = strcat(pathToHandle, ";");
            strremove(pathEnv, pathToHandle);
        }
        printf("Deleted path: %s\n", pathToHandle);
    }
    else
    {
        printf("Added path: %s\n", pathToHandle);
        pathEnv = strcat(pathEnv, ";");
        pathEnv = strcat(pathEnv, pathToHandle);
    }
    SetString(hKey, "Path", pathEnv);

    RegCloseKey(hKey);
}
