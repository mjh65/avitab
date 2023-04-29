
var AvitabIsLoaded = false;
document.addEventListener('beforeunload', function () {
    AvitabIsLoaded = false;
}, false);

class AvitabElement extends TemplateElement {
    constructor() {
        super(...arguments);
        this.panelActive = false;
        this.ingameUi = null;
        this.display = null;
        this.canvas = null;
        this.canvasX = 800;
        this.canvasY = 480;
        this.frameBuffer = null;
        this.response = null;
        this.isResponsePending = false;
        this.loopTime = 0;
        this.responseTimer = 0;
        this.stateCounter = 0;
        this.mouseDown = false;
        this.wheelPending = 0;
    }
    connectedCallback() {
        //console.log('connectedCallback()');
        super.connectedCallback();

        var self = this;
        this.ingameUi = this.querySelector('ingame-ui');
        this.display = document.getElementById("AvitabDisplay");
        this.canvas = document.getElementById("AvitabCanvas");
        this.frameBuffer = document.getElementById("FrameBuffer");
        this.response = document.getElementById("Response");

        if (this.ingameUi) {
            this.ingameUi.addEventListener("panelActive", (e) => {
                //console.log('panelActive');
                self.panelActive = true;
                this.loopTime = Date.now();
                let updateLoop = () => {
                    if (window["IsDestroying"] === true) {
                        return;
                    }
                    if (!AvitabIsLoaded) {
                        return;
                    }
                    this.flightLoop();
                    requestAnimationFrame(updateLoop);
                };
                AvitabIsLoaded = true;
                requestAnimationFrame(updateLoop);
            });
            this.ingameUi.addEventListener("panelInactive", (e) => {
                //console.log('panelInactive');
                self.panelActive = false;
            });
            this.ingameUi.addEventListener("OnResize", this.onPanelResized.bind(this));
        }

        if (this.canvas) {
            this.canvas.addEventListener("mousemove", (e) => {
                if (self.mouseDown) {
                    this.mouseEventReq(self, e.offsetX, e.offsetY, 1);
                }
            });
            this.canvas.addEventListener("mousedown", (e) => {
                self.mouseDown = true;
                this.mouseEventReq(self, e.offsetX, e.offsetY, 1);
            });
            this.canvas.addEventListener("mouseup", (e) => {
                self.mouseDown = false;
                this.mouseEventReq(self, e.offsetX, e.offsetY, 0);
            });
            this.canvas.addEventListener("wheel", (e) => {
                this.wheelEventReq(self, e.deltaY);
            });
        }

        if (this.frameBuffer) {
            this.frameBuffer.addEventListener("load", this.frameReady(self));
            this.frameBuffer.addEventListener("error", this.noConnection(self));
            this.frameBuffer.style.display = "none";
        }

        if (this.response) {
            this.response.style.display = "none";
        }
    }
    disconnectedCallback() {
        //console.log('disconnectedCallback()');
        super.disconnectedCallback();
    }

    onPanelResized() {
        //console.log("Resize: " + this.display.offsetHeight + " x " + this.display.offsetHeight);
        this.canvasX = this.display.offsetWidth;
        this.canvasY = this.display.offsetHeight;
        this.canvas.width = this.canvasX;
        this.canvas.height = this.canvasY;
    }

    mouseEventReq(me,x,y,b) {
        let xhttp = new XMLHttpRequest();
        let url = "http://127.0.0.1:26730/m?t=" + me.loopTime;
        url = url + "&mx=" + Math.round(x * 800 / me.canvasX) + "&my=" + Math.round(y * 480 / me.canvasY) + "&mb=" + b;
        xhttp.onreadystatechange = function() {
            if (this.readyState == 4 && this.status == 200) {
                me.mouseEventResp(me)
            }
        };
        xhttp.open("GET", url);
        xhttp.send();
    }
    mouseEventResp(me) {
        //console.log("mouseEventResp");
    }

    wheelEventReq(me,d) {
        let xhttp = new XMLHttpRequest();
        let url = "http://127.0.0.1:26730/m?t=" + me.loopTime + ((d<0) ? "&wu" : "&wd");
        xhttp.onreadystatechange = function() {
            if (this.readyState == 4 && this.status == 200) {
                me.wheelEventResp(me)
            }
        };
        xhttp.open("GET", url);
        xhttp.send();
    }
    wheelEventResp(me) {
        //console.log("wheelEventResp");
    }

    frameReady(me) {
        //console.log('frameReady');
        let ctx = me.canvas.getContext("2d");
        ctx.drawImage(me.frameBuffer, 0, 0, 800, 480, 0, 0, me.canvasX, me.canvasY);
        me.isResponsePending = false;
    }
    noConnection(me) {
        //console.log('noConnection');
        let ctx = me.canvas.getContext("2d");
        ctx.fillStyle = "red";
        ctx.strokeStyle = "red";
        ctx.lineWidth = 20;
        ctx.beginPath();
        ctx.arc(400, 160, 100, 0, 2 * Math.PI);
        ctx.moveTo(470, 90);
        ctx.lineTo(330, 230);
        ctx.stroke();
        ctx.font = "36px Arial";
        ctx.fillStyle = "black";
        ctx.fillText("!no response from Avitab-msfs-igps.exe!", 70, 350);
        me.isResponsePending = false;
    }

    flightLoop() {
        const now = Date.now();
        const dt = now - this.loopTime;
        this.loopTime = now;

        if (this.isResponsePending) {
            this.responseTimer += dt;
            if (this.responseTimer > 2000) {
                // if no response after 2s assume failure and try again
                this.noConnection(this);
            } else if (this.frameBuffer.complete) {
                // in case the image load callback didn't fire
                this.frameReady(this);
            }
            return;
        }

        //console.log("flightLoop(delta:" + dt + ")"); // deltas are usually 30 (ms) or so

        const frameUpdateRate = 50;
        this.stateCounter += 1;
        if (this.stateCounter > frameUpdateRate) {
            // get a new frame from the server, every 600ms or so if nothing else is happening,
            // and reset the counter to start a new cycle
            this.stateCounter = 0;
            let url = "http://127.0.0.1:26730/f?t=" + this.loopTime;
            this.isResponsePending = true;
            this.responseTimer = 0;
            this.frameBuffer.src = url;
        } else if (this.stateCounter = frameUpdateRate) {
            // update the aircraft position just before we ask for a new frame
            const lat = SimVar.GetSimVarValue("PLANE LATITUDE", "degree latitude");
            const lon = SimVar.GetSimVarValue("PLANE LONGITUDE", "degree longitude");
            const alt = SimVar.GetSimVarValue("PLANE ALTITUDE", "feet");
            const hdg = SimVar.GetSimVarValue("PLANE HEADING DEGREES TRUE", "radians");
            let url = "http://127.0.0.1:26730/m?t=" + this.loopTime + "&lt=" + lat + "&ln=" + lon + "&al=" + alt + "&hg=" + hdg;
            this.isResponsePending = true;
            this.responseTimer = 0;
            this.response.src = url;
        }
    }
}
window.customElements.define("aviators-tablet", AvitabElement);
checkAutoload();
