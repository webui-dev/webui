// Copyright (c) 2011-2018, Anders Evenrud <andersevenrud@gmail.com>

! function(n) {
    var t = {};

    function e(r) {
        if (t[r]) return t[r].exports;
        var o = t[r] = {
            i: r,
            l: !1,
            exports: {}
        };
        return n[r].call(o.exports, o, o.exports, e), o.l = !0, o.exports
    }
    e.m = n, e.c = t, e.d = function(n, t, r) {
        e.o(n, t) || Object.defineProperty(n, t, {
            enumerable: !0,
            get: r
        })
    }, e.r = function(n) {
        "undefined" != typeof Symbol && Symbol.toStringTag && Object.defineProperty(n, Symbol.toStringTag, {
            value: "Module"
        }), Object.defineProperty(n, "__esModule", {
            value: !0
        })
    }, e.t = function(n, t) {
        if (1 & t && (n = e(n)), 8 & t) return n;
        if (4 & t && "object" == typeof n && n && n.__esModule) return n;
        var r = Object.create(null);
        if (e.r(r), Object.defineProperty(r, "default", {
                enumerable: !0,
                value: n
            }), 2 & t && "string" != typeof n)
            for (var o in n) e.d(r, o, function(t) {
                return n[t]
            }.bind(null, o));
        return r
    }, e.n = function(n) {
        var t = n && n.__esModule ? function() {
            return n.default
        } : function() {
            return n
        };
        return e.d(t, "a", t), t
    }, e.o = function(n, t) {
        return Object.prototype.hasOwnProperty.call(n, t)
    }, e.p = "", e(e.s = 2)
}([function(n, t) {
    n.exports = function(n) {
        return n && n.__esModule ? n : {
            default: n
        }
    }
}, function(n, t, e) {
    var r = e(4),
        o = e(5),
        c = e(6);
    n.exports = function(n) {
        return r(n) || o(n) || c()
    }
}, function(n, t, e) {
    n.exports = e(3)
}, function(n, t, e) {
    "use strict";
    var o = e(9),
        c = {
            email: "andersevenrud@gmail.com",
            twitter: "https://twitter.com/andersevenrud",
            github: "https://github.com/andersevenrud"
        },
        i = Object.keys(c).reduce(function(n, t) {
            return n.concat(["".concat(t, " - ").concat(c[t])])
        }, []).join("\n"),
        a = "\nCreated by Anders Evenrud\n\n".concat(i, "\n\nUse ex. 'contact twitter' to open the links.\n")
    ;
    document.addEventListener("DOMContentLoaded", function() {
        var n = (0, o.terminal)({
            prompt: function() {
                return "$ "
            },
            banner: "\ndP   dP   dP          dP       dP     dP dP \n88   88   88          88       88     88 88 \n88  .8P  .8P .d8888b. 88d888b. 88     88 88 \n88  d8'  d8' 88ooood8 88'  `88 88     88 88 \n88.d8P8.d8P  88.  ... 88.  .88 Y8.   .8P 88 \n8888' Y88'   `88888P' 88Y8888' `Y88888P' dP\n\nAnderShell 3000 (Python)\n\nWebUI v2.3.0 | webui.me\n\n"
        })
    })
}, function(n, t) {
    n.exports = function(n) {
        if (Array.isArray(n)) {
            for (var t = 0, e = new Array(n.length); t < n.length; t++) e[t] = n[t];
            return e
        }
    }
}, function(n, t) {
    n.exports = function(n) {
        if (Symbol.iterator in Object(n) || "[object Arguments]" === Object.prototype.toString.call(n)) return Array.from(n)
    }
}, function(n, t) {
    n.exports = function() {
        throw new TypeError("Invalid attempt to spread non-iterable instance")
    }
}, function(n, t, e) {}, , function(n, t, e) {
    "use strict";
    var r = e(0);
    Object.defineProperty(t, "__esModule", {
        value: !0
    }), t.terminal = void 0;
    var o = r(e(1)),
        c = function(n) {
            var t = n.value.length;
            if (n.setSelectionRange) n.focus(), n.setSelectionRange(t, t);
            else if (n.createTextRange) {
                var e = n.createTextRange();
                e.collapse(!0), e.moveEnd("character", t), e.moveStart("character", t), e.select()
            }
        };
    t.terminal = function(n) {
        var t = [],
            e = !1,
            r = function(n) {
                return Object.assign({}, {
                    banner: "Hello World",
                    prompt: function() {
                        return "$ > "
                    },
                    tickrate: 1e3 / 60,
                    buflen: 8,
                    commands: {}
                }, n || {})
            }(n),
            i = r.prompt,
            a = r.banner,
            u = r.commands,
            l = r.buflen,
            f = r.tickrate,
            d = document.querySelector("#terminal"),
            s = function(n) {
                var t = document.createElement("textarea");
                return t.contentEditable = !0, t.spellcheck = !1, t.value = "", n.appendChild(t), t
            }(d),
            p = function(n) {
                return parseInt(window.getComputedStyle(n).getPropertyValue("font-size"), 10)
            }(s),
            m = s.offsetWidth,
            v = Math.round(m / p * 1.9),
            y = function(n, e) {
                var r = n.split(/\n/);
                e && (r = r.map(function(n) {
                    return n.length > 0 ? n.padStart(n.length + (v / 2 - n.length / 2), " ") : n
                }));
                var o = r.join("\n") + "\n" + i();
                t = t.concat(o.split(""))
            },
            h = function(n, t) {
                return function(e) {
                    if (e.length > 0) {
                        var r = Math.min(t, e.length),
                            o = e.splice(0, r);
                        return n.value += o.join(""), c(n), n.scrollTop = n.scrollHeight, !0
                    }
                    return !1
                }
            }(s, l),
            g = function(n) {
                return function(t) {
                    for (var e = arguments.length, r = new Array(e > 1 ? e - 1 : 0), o = 1; o < e; o++) r[o - 1] = arguments[o];
                    return function(e) {
                        try {
                            webui_fn('Run', _cmd).then((response) => {
                                n[t] ? e(n[t].apply(n, r) + "\n") : e(response)
                            });
                        } catch (n) {
                            console.warn(n), e("Exception: ".concat(n, "\n"))
                        }
                    }
                }
            }(u),
            b = function(n, t) {
                var e = 0;
                return function r(o) {
                    var c = performance.now(),
                        i = c - e;
                    i > n && (e = c - i % n, t()), window.requestAnimationFrame(r)
                }
            }(f, function() {
                return e = h(t)
            }),
            w = function(n) {
                return function(t) {
                    if (t.length) {
                        var e = t.split(" ").map(function(n) {
                                return n.trim()
                            }),
                            r = e.splice(0, 1)[0];
                        console.debug(r, e), n.apply(void 0, [r].concat((0, o.default)(e)))
                    }
                }
            }(function(n) {
                for (var t = arguments.length, e = new Array(t > 1 ? t - 1 : 0), r = 1; r < t; r++) e[r - 1] = arguments[r];
                return g.apply(void 0, [n].concat(e))(y)
            }),
            j = function() {
                return setTimeout(function() {
                    return s.focus()
                }, 1)
            },
            k = function(n) {
                var t = [],
                    e = {
                        8: "backspace",
                        13: "enter"
                    },
                    r = function(n) {
                        return e[n.which || n.keyCode]
                    };
                return {
                    keypress: function(e) {
                        if ("enter" === r(e)) {
                            var o = t.join("").trim();
                            _cmd = String(o);
                            n(o), t = []
                        } else "backspace" !== r(e) && t.push(String.fromCharCode(e.which || e.keyCode))
                    },
                    keydown: function(n) {
                        "backspace" === r(n) ? t.length > 0 ? t.pop() : n.preventDefault() : function(n) {
                            return n >= 33 && n <= 40
                        }(n.keyCode) && n.preventDefault()
                    }
                }
            }(w),
            E = function(n) {
                return e ? n.preventDefault() : k[n.type](n)
            };
        return s.addEventListener("focus", function() {
            return c(s)
        }), s.addEventListener("blur", j), s.addEventListener("keypress", E), s.addEventListener("keydown", E), window.addEventListener("focus", j), d.addEventListener("click", j), d.appendChild(s), b(), y(a, !0), j(), {
            focus: j,
            parse: w,
            clear: function() {
                return s.value = ""
            },
            print: y
        }
    }
}]);

// Hold the full command
var _cmd = "";