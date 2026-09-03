---
title: "Information Security"
tags: [it, security]
draft: false
---

# Information Security

----
## Use .netrc to Store Login Credentials

```bash
$ vi ~/.netrc

machine <server_url>
login <account>
password <token_or_password>

$ chmod 600 ~/.netrc
```

----
## Proxy Settings

Check proxy settings

```bash
$ env | grep -i proxy
$ grep -R "proxy" ~/.bashrc ~/.profile ~/.bash_profile ~/.zshrc /etc/environment /etc/profile /etc/profile.d/* 2>/dev/null
```

Check proxy settings on Windows

```bash
> New-SmbMapping -LocalPath "Q:" -RemotePath "\\10.175.177.149\mike" -TcpPort 4100 Credential
> NET USE Q: \\10.175.177.149\mike /TCPPORT:4100
> netsh interface portproxy add v4tov4 listenaddress=10.175.177.149 listenport=445 connectaddress=10.175.177.149 connectport=4100

> netsh interface portproxy show all
> netsh interface portproxy delete v4tov4 listenport=445 listenaddress=127.0.0.1
```

```bash
$ netsh interface portproxy show v4tov4
```

----
## Problems

>   x509: certificate signed by unknown authority

檢查 OpenSSL certificate chain

```bash
# s: Subject, 憑證主體(發給誰)  
# i: Issuer, 憑證簽發者(誰發的)  
$ openssl s_client -connect <target_url>:443 -showcerts

Certificate chain
 0 s:C = US, ST = California, O = MyCompany, CN = artifactory.keen.tech
   i:C = US, O = MyCompany, CN = MyCompany Internal CA
 1 s:C = US, O = MyCompany, CN = MyCompany Internal CA
   i:C = US, O = Root CA, CN = MyCompany Root CA
```

可能錯誤1: 缺少簽發該憑證的根憑證/中間憑證  
Verification error: unable to get local issuer certificate

可能錯誤2: 鏈中有自簽憑證但系統預設不信任  
Verification error: self signed certificate in certificate chain

解決方案

```bash
# 安裝 foo.crt 到系統
$ sudo cp foo.crt /usr/local/share/ca-certificates/
$ sudo update-ca-certificates
```

可能錯誤3: no peer certificate available  

```bash
# add '-tls1_3' and test again
$ openssl s_client -connect <target_url>:443 -tls1_3 -showcerts

# add curve and test again
$ openssl s_client -connect <target_url>:443 -curves X25519:P-256 -showcerts
```

可能錯誤4: Verify return code: 20 (unable to get local issuer certificate)  
缺少 `DigiCert G2 TLS EU RSA4096 SHA384 2022 CA1`

```text
depth=1 C = IE, O = DigiCert Ireland Limited, CN = DigiCert G2 TLS EU RSA4096 SHA384 2022 CA1
verify error:num=20:unable to get local issuer certificate
```

解決方案

```bash
# 1. Update the OS CA certificate list.
$ sudo apt-get update
$ sudo apt-get install --only-upgrade ca-certificates

# 2. Download and install the CA manually.
$ curl -o digicert_root_g2.crt https://cacerts.digicert.com/DigiCertGlobalRootG2.crt.pem
# test connection with this CA
$ openssl s_client -connect artifactory.keen.tech:443 -CAfile digicert_root_g2.crt
# trust this CA
$ git config http.sslCAInfo $(pwd)/digicert_root_g2.crt
```