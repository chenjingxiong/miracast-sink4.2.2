
#ifndef A_NETWORK_SESSION_H_

#define A_NETWORK_SESSION_H_

#include <media/stagefright/foundation/ABase.h>
#include <utils/KeyedVector.h>
#include <utils/RefBase.h> //结合sp和wp，实现了一套通过引用计数的方法来控制对象生命周期的机制
#include <utils/Thread.h>

#include <netinet/in.h>

namespace android {

struct AMessage;

// Helper class to manage a number of live sockets (datagram and stream-based)
// on a single thread. Clients are notified about activity through AMessages.
struct ANetworkSession : public RefBase { 

	//ReBase 结合sp和wp，实现了一套通过引用计数的方法来控制对象生命周期的机制
    ANetworkSession();

    status_t start();
    status_t stop();

    status_t createRTSPClient(
            const char *host, unsigned port, const sp<AMessage> &notify,
            int32_t *sessionID);

    status_t createRTSPServer(
            const struct in_addr &addr, unsigned port,
            const sp<AMessage> &notify, int32_t *sessionID);

    status_t createUDPSession(
            unsigned localPort, const sp<AMessage> &notify, int32_t *sessionID);

    status_t createUDPSession(
            unsigned localPort,
            const char *remoteHost,
            unsigned remotePort,
            const sp<AMessage> &notify,
            int32_t *sessionID);

    status_t connectUDPSession(
            int32_t sessionID, const char *remoteHost, unsigned remotePort);

    // passive
    status_t createTCPDatagramSession(
            const struct in_addr &addr, unsigned port,
            const sp<AMessage> &notify, int32_t *sessionID);

    // active
    status_t createTCPDatagramSession(
            unsigned localPort,
            const char *remoteHost,
            unsigned remotePort,
            const sp<AMessage> &notify,
            int32_t *sessionID);

    status_t destroySession(int32_t sessionID);

    status_t sendRequest(
            int32_t sessionID, const void *data, ssize_t size = -1);

    enum NotificationReason {
        kWhatError,
        kWhatConnected,
        kWhatClientConnected,
        kWhatData,
        kWhatDatagram,
        kWhatBinaryData,
    };

protected:
    virtual ~ANetworkSession();

private:
    struct NetworkThread;
    struct Session;

    Mutex mLock;
    sp<Thread> mThread;

    int32_t mNextSessionID;

    int mPipeFd[2];

    KeyedVector<int32_t, sp<Session> > mSessions;

    enum Mode {
        kModeCreateUDPSession,
        kModeCreateTCPDatagramSessionPassive,
        kModeCreateTCPDatagramSessionActive,
        kModeCreateRTSPServer,
        kModeCreateRTSPClient,
    };
    status_t createClientOrServer(
            Mode mode,
            const struct in_addr *addr,
            unsigned port,
            const char *remoteHost,
            unsigned remotePort,
            const sp<AMessage> &notify,
            int32_t *sessionID);

    void threadLoop();
    void interrupt();

    static status_t MakeSocketNonBlocking(int s);

    DISALLOW_EVIL_CONSTRUCTORS(ANetworkSession);
};

}  // namespace android

#endif  // A_NETWORK_SESSION_H_
