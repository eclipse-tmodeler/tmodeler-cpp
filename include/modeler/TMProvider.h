#ifndef TMPROVIDER_H
#define TMPROVIDER_H

class TMProvider {
public:
    virtual void onServerStatusChange(bool isConnected) = 0;
    virtual void onCommit() = 0;
    virtual ~TMProvider() = default;
};

#endif // TMPROVIDER_H