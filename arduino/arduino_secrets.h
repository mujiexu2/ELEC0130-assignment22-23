// Fill in  your WiFi networks SSID and password
#define SECRET_SSID "MyLaptop"
#define SECRET_PASS "66666666"

// Fill in the hostname of your AWS IoT broker
#define SECRET_BROKER "a3l5bqqa7x3v5l-ats.iot.eu-west-2.amazonaws.com"

// Fill in the boards public certificate
const char SECRET_CERTIFICATE[] = R"(
-----BEGIN CERTIFICATE-----
MIICgTCCAWmgAwIBAgIVANImDWlyaD++wyxUGCUvcD2dJsVhMA0GCSqGSIb3DQEB
CwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t
IEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0yMzAzMDMwMDI3
MDVaFw00OTEyMzEyMzU5NTlaMBAxDjAMBgNVBAMTBU15TUtSMFkwEwYHKoZIzj0C
AQYIKoZIzj0DAQcDQgAEcOitbuJoTZcz+6xtc4O+az0u9hueEZv1MFCqHaKJqsmX
VxFmXgIiXLavoBZdObzoU9DOUagVIKmKX3EaTFxxNaNgMF4wHwYDVR0jBBgwFoAU
E2TqwmNTaIgyAq8hCMc9Ol8OQjcwHQYDVR0OBBYEFJG43HsloValJbh9sFoDnC3w
7A5uMAwGA1UdEwEB/wQCMAAwDgYDVR0PAQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUA
A4IBAQBirdOmT2wsKbgwIwdWWcitEwxGZj7vlHY6qNlkoSL7bAPd37QQJZ6r3L+L
3ckR9LKuWOsJTyvV6c1s4NcO80mB8mv5QS7KqbM8ErztMXoNzWYiYVKWYhZ+d/Du
XrSo3Ay1ABXRzrP4KvhGR7DsYJZLBLckO/tYN/l8q4/PltXfaM3VqZ2rVJ6UXGtV
ZhyFGOrRTa/PNukoL/X+cJHEDbX2OSLM9MgyFEsxoE90G97wGP/y6E9WIFmRgKor
1Dz/V5wdx4KqnNjSeB0NfnfhUHBDhdrVWPs6b6Y1vODbz1imXO9rA1adDBMoQoSB
OaECB1Ghz/WURFVsTIinTM1iOTft
-----END CERTIFICATE-----

)";
