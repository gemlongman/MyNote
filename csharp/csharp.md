# 引用 c

```c++

extern "C"
__declspec(dllexport) char * gettest(char* url){
    std::string in = std::string(url);
    char* scanReturn;
    scanReturn = (char*)malloc((sizeof(char) * in.size()) + 1);
    strcpy(scanReturn, in.c_str());
    return scanReturn;
}
```

```c#

[DllImport("IOATicket.dll", CharSet = CharSet.Ansi)] //引入dll，并设置字符集 
public static extern IntPtr gettest(string url);
        
// https://stackoverflow.com/questions/14953180/calling-c-dll-functions-returning-char-from-c-cannot-use-dllimport
IntPtr verText = gettest(string url);
string s = Marshal.PtrToStringAnsi(verText);
Console.WriteLine(s);
```
