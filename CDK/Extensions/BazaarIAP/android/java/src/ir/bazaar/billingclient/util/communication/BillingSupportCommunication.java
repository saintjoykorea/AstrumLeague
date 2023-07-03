package ir.bazaar.billingclient.util.communication;


import ir.bazaar.billingclient.util.IabResult;

public interface BillingSupportCommunication {
    void onBillingSupportResult(int response);
    void remoteExceptionHappened(IabResult result);
}
