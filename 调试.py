import socket,struct,json

HEAD_MODE = False

def asd(anys):
    strs = json.dumps(anys)
    if HEAD_MODE:
        strss = struct.pack("Q",len(strs.encode('gb2312')))+strs.encode('gb2312')
    else:
        strss = strs.encode('gb2312')
    
    s.sendall(strss)
    print(json.loads(s.recv(1024).decode('gb2312')))

s = socket.socket()
s.connect(("127.0.0.1",1234))
while True:
    asd({"get_version":{}})
    asd({"creat_new_money":{"name":"1p","value":"10","creator":"quqi"}})
    asd({"creat_new_money":{"name":"2p","value":"10","creator":"quqi"}})

    asd({"creat_new_user":{"name":"quqi","uuid":"asdasdasd"}})
    asd({"change_user_money":{"name":"quqi","money_name":"1p","set":"1000"}})
    asd({"get_exchange_rate":{"money_name1":"2p","money_name2":"1p"}})
    asd({"get_user_money":{"name":"quqi"}})