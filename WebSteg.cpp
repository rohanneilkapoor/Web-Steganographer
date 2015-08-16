#include "provided.h"
#include <string>
#include "http.h"
using namespace std;


bool WebSteg::hideMessageInPage(const string& url, const string& msg, string& host)
{
    string a;
    if(HTTP().get(url,a) && Steg::hide(a,msg,host))
        return true;
    return false;
}

bool WebSteg::revealMessageInPage(const string& url, string& msg)
{
    string x;
    if(HTTP().get(url,x) && Steg::reveal(x,msg))
        return true;
    return false;
}
