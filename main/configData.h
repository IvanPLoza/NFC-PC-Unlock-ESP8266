#ifndef SETTINGS_H
#define SETTINGS_H

class configData {
    
    public:
        configData();
        int getError(String &s);

    private:
        int error;
        String errmsg;
 
    //////////////////////////////////////////////////////////////////////////
    /*
        This is one of the places where you would customize this class to be 
        used with your application specific configuration data.
        
        The other place is in configData.cpp
    */
    public:
        char *getEMAIL();
        String getEMAILString();
        
        char *getPASS();
        String getPASSString();
        
        int getUID();
        
    private:
        String email;
        String pass;
        int uid;
};

#endif // SETTINGS_H

