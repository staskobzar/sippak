# sippak
## SIP command line utility

Simple command line utility for SIP protocol based on PJPROJECT.

### Install

Requires pjproject library.

```
make
sudo make install
```

### Usage

```
  COMMAND:
  Default commans is "PING".
    PING      Send OPTIONS packet to destination.
    PUBLISH   Send PUBLISH events and status. Default document is 'pidf' and default event is 'presence'.
    SUBSCRIBE Send SUBSCRIBE request. Default event is 'presence'
    NOTIFY    Send NOTIFY request. Default event is 'keep-alive'
    REGISTER  AOR contacts list, register or unregister.
    REFER     Send REFER method outside dialog. Implements click-to-dial scenario as in RFC5359 #2.18.
              This command requires parameter --to for Refer-To header.
    MESSAGE   Send MESSAGE method with text. SIP instant messaging.
    INVITE    Initiates and handles INVITE session. After session is confirmed (200) sends BYE.

  OPTIONS:
    -h, --help      Print this usage message and exit.
    -V, --version   Print version and exit.
    -v, --verbose   Verbosity increase. Short option can be repeated multiple times.
                    Long option can have value. Example: --verbose=6
    -q, --quiet     Silent or quiet mode. Mute sippak.
    --ns=LIST       Define DNS nameservers to use. Comma separated list up to 3 servers.
                    Can be defined with ports. If ports are not defined will use default port 53.
                    For example: --ns=8.8.8.8 or --ns=4.4.4.4:553,3.3.3.3
    --color         Enable colorized output. Disabled by default.
    --trail-dot     Output trailing dot '.' at the end of each SIP message line.
    --log-time      Print time and microseconds in logs.
    --log-level     Print log level: ERROR, INFO etc.
    --log-snd       Print log sender file or module name.
    -P, --local-port=PORT
                    Bind local port. Default is random port.
    -l, --local-host=HOST|IP
                    Bind local hostname or IP. Default is first available local inface.
    -u, --username=USER
                    Username part in Authentication as well as in Contact and
                    From header URI. Default is from destination URI.
    -p, --password=PASS
                    Password for authentication.
    -c, --contact=CONTACT
                    Custom contact header value. Must be valid SIP URI. For example: sip:alice@10.10.10.1:123
                    If not set, contact header is generated automatically.
    -F, --from-name=DISPLAY_NAME
                    Display name in From header. Default is empty.
    -t, --proto=PROTO
                    Transport protocol to use. Possible values 'tcp' or 'udp'. Default is 'udp'.
    -X, --expires=NUMBER
                    Expires header value. Must be number more then 0.
    --pres-status=STATUS
                    Presence status for PUBLISH command. STATUS value can be 'open', 'closed' etc.
                    If this parameter is not defined or invalid, will use 'open' status.
    --pres-note=MESSAGE
                    Presence note message string for PUBLISH command.
    -C, --content-type=TYPE
                    Publish or notify content type. TYPE values can be pidf, xpidf or mwi.
                    Note: XPIDF implementation is not complete in pjproject.
    -E, --event=EVENT
                    Presence event header for subscribe, publish or notify methods.
                    EVENT values can be "presence", "message-summary", "keep-alive" etc.
                    For convinence, there is an alias "mwi" can be used for "message-summary" event.
                    For PUBLISH and SUBSCRIBE method default value is 'presence'.
                    For NOTIFY method default value is 'keep-alive'.
    -M, --mwi=N,N,N,N
                    Voice messages list. Comma separated list of numbers.
                    List of messages new,old,urgent_new,urgent_old.
                    List from 1 to 4 members. Not set members will be initiated with 0.
    --mwi-acc=ACCOUNT
                    Voicemail account for message-summary body. If not set then destination URI used.
    --clist
                    Flag for REGISTER method to get list of contacts registered for AOR.
    --cancel-all
                    Flag for REGISTER method to cancel all registrations for AOR.
    --cancel
                    Cancel registarations for REGISTER or session for INVITE.
                    When used with REGISTER method then cancels contact registration for AOR.
                    Contact field can be set with --contact option or will be generated.
                    When used with INVITE will cancel session in early state.
    --to=SIP_URI
                    Parameter for REFER command to setup Refer-To header value.
    --body=TEXT
                    Parameter for MESSAGE command to setup message body text.
    --codec=LIST
                    Set codec or codecs to use during INVITE session. Value can be
                    a single codec name or comma-separated list of codecs. By default
                    sippak will use codec g711. Codec names are case-insensitive.
                    Following codec names can be used:
                    speex,ilbc,gsm,g711,g722,ipp,l16,amr,silk,opus,bcg729,all
                    When set to "all" will try to setup all available codecs for media.
    --rtp-port=PORT
                    Port to use for media streams and negotiate with SDP.
    -A, --user-agent=STRING
                    Set User-Agent SIP header value.
    -H, --header=HEADER
                    Add custom header to request. Multiple custom headers (up to 12) can be added.
                    Parameter value must contain header name and value separated by colon.
                    Examples:
                    --header="X-Foo: bar", -H X-Foo:bar, -H "X-Assert: sip:123@sip.com"
    -R, --proxy=PROXY
                    Add proxy to request. Multiple proxies (up to 12) can be added.
                    The first proxy will be used as outbound proxy where the request will be sent.
                    All additional proxies will be added as Route headers to the request.
                    Supported methods are INVITE, REGISTER and PUBLISH.
                    Examples:
                    --proxy=sip:sip.com:2585, -R sip:10.23.24.100:6060;lr

```


