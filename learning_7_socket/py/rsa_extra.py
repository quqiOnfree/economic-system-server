from py.lib.Crypto import Random
from py.lib.Crypto.PublicKey import RSA
from py.lib.Crypto.Cipher import PKCS1_v1_5
from py.lib.Crypto.Signature import pkcs1_15
from py.lib.Crypto.Hash import SHA256
import py.lib.base64 as base64


def rsa_key(num: int):
    key = RSA.generate(num, Random.new().read)
    return key.publickey().export_key(), key.export_key()


def dersa(encrypted_data: bytes, signature: bytes, public_key: bytes, private_key: bytes, size: int):
    encrypted_data = base64.b64decode(encrypted_data)
    signature = base64.b64decode(signature)
    lenth = int(size/1024*128)
    priobj = PKCS1_v1_5.new(RSA.import_key(private_key))
    res = b""
    for i in range(0, len(encrypted_data), lenth):
        res += priobj.decrypt(encrypted_data[i:i+lenth], "xyz")
    try:
        pkcs1_15.new(RSA.import_key(public_key)).verify(
            SHA256.new(res), signature)
        return res, True
    except:
        return res, False


def enrsa(data: bytes, public_key: bytes, private_key: bytes, size: int):
    lenth = int(size/1024*100)
    pubobj = PKCS1_v1_5.new(RSA.import_key(public_key))
    res = b""
    for i in range(0, len(data), lenth):
        res += pubobj.encrypt(data[i:i+lenth])
    return base64.b64encode(res), base64.b64encode(pkcs1_15.new(RSA.import_key(private_key)).sign(SHA256.new(data)))
