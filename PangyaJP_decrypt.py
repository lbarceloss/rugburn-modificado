#!/usr/bin/env python3
"""
Decryptor do config .ini do nProtect GameGuard (formato [GAMEMON]).
Descoberto 2026-07-13 via engenharia reversa (repo fatrolls/nProtect-GameGuard).

Estrutura do arquivo:
  [ corpo cifrado RC4 (len bytes) ][ filename ASCII (fnsz) ][ assinatura RSA-512 (sigsz) ][ trailer 16B ]
  trailer = <u32 magic1=0x32812622> <u32 filename_size> <u32 signature_size> <u32 magic2=0x32812621>

Cripto: RC4, chave = CryptDeriveKey(MS Base Provider, CALG_RC4, MD5("ectGameMon"))
        = MD5("ectGameMon")[:5] + 11 bytes 0x00   (RC4 40-bit com salt zero)
A assinatura RSA-512 (chave pública embutida no npggNT.des) só valida integridade;
re-encriptar exigiria a chave PRIVADA da INCA Internet.

Uso:  python gg_ini_decrypt.py <arquivo.ini> [saida.txt]
"""
import sys, struct, hashlib

PBDATA = b"ectGameMon"                       # "senha" que o GameGuard passa ao decryptor
MAGIC1, MAGIC2 = 0x32812622, 0x32812621

def rc4(key: bytes, data: bytes) -> bytes:
    S = list(range(256)); j = 0
    for i in range(256):
        j = (j + S[i] + key[i % len(key)]) & 0xff
        S[i], S[j] = S[j], S[i]
    out = bytearray(); i = j = 0
    for b in data:
        i = (i + 1) & 0xff; j = (j + S[i]) & 0xff
        S[i], S[j] = S[j], S[i]
        out.append(b ^ S[(S[i] + S[j]) & 0xff])
    return bytes(out)

def decrypt_gg_ini(raw: bytes):
    n = len(raw)
    magic1, fnsz, sigsz, magic2 = struct.unpack("<IIII", raw[n-16:n])
    if magic1 != MAGIC1 or magic2 != MAGIC2:
        raise ValueError("nao e um .ini do GameGuard (magic invalido)")
    body_len = n - (fnsz + sigsz + 16)
    key = hashlib.md5(PBDATA).digest()[:5] + bytes(11)   # RC4 key de 40 bits
    text = rc4(key, raw[:body_len])
    filename = raw[body_len:body_len+fnsz].split(b"\x00")[0].decode("latin1")
    signature = raw[body_len+fnsz:body_len+fnsz+sigsz]
    return text, filename, signature

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print(__doc__); sys.exit(1)
    raw = open(sys.argv[1], "rb").read()
    text, filename, sig = decrypt_gg_ini(raw)
    # o config real termina quando comeca padding binario; corta na ultima linha "chave=valor"
    lines = text.split(b"\r\n")
    clean = []
    for ln in lines:
        if b"=" in ln and all(32 <= c < 127 for c in ln.split(b"=", 1)[0]):
            clean.append(ln.decode("latin1"))
        elif ln.strip() in (b"", b"[GAMEMON]") or ln.startswith(b"["):
            clean.append(ln.decode("latin1", "replace"))
        else:
            break
    out = "\r\n".join(clean)
    print(f"[filename embutido: {filename} | assinatura RSA-512: {len(sig)} bytes]\n")
    print(out)
    if len(sys.argv) >= 3:
        open(sys.argv[2], "w", encoding="utf-8", newline="").write(out)
        print(f"\n-> salvo em {sys.argv[2]}")
