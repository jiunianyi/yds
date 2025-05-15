#pragma once
#include<grpcpp/grpcpp.h>
#include"message.grpc.pb.h"
#include"const.h"

using grpc::Channel;//һ�����������������ͨ�ŵ�ͨ��
using grpc::Status;//Զ�̵��õ�ִ��״̬�������ɹ��͸��ִ����������ͨ�����ڷ��ظ��ͻ���һ��������ʹ�����Ϣ��
using grpc::ClientContext;//������

using message::GetVarifyReq;
using message::GetVarifyRsp;//�ذ�
using message::varifyService;

class RPComPool {
public:
    RPComPool(std::size_t poolsize, std::string host, std::string port);
    ~RPComPool();
    void close();
    std::unique_ptr<varifyService::Stub> getconnection();
    void returnconnection(std::unique_ptr<varifyService::Stub> context);
private:
    std::atomic<bool> b_stop_;
    std::size_t poolsize_;
    std::string host_;
    std::string port_;
    std::queue<std::unique_ptr<varifyService::Stub>> connections_;
    std::mutex mutex_;
    std::condition_variable cond_;
};
class VerifyGrpcClient:public Singleton<VerifyGrpcClient>
{
	friend class Singleton<VerifyGrpcClient>;
public:
    GetVarifyRsp GetVarifyCode(std::string email) {
        ClientContext context;
        GetVarifyRsp reply;
        GetVarifyReq request;
        request.set_email(email);
        auto stub_ = pool_->getconnection();
        Status status = stub_->GetVarifyCode(&context, request, &reply);
        if (status.ok()) {
            pool_->returnconnection(std::move(stub_));
            return reply;
        }
        else {
            pool_->returnconnection(std::move(stub_));
            reply.set_error(ErrorCodes::RPCFailed);
            return reply;
        }
    }
private:
    VerifyGrpcClient();
    std::unique_ptr<RPComPool> pool_;
};

