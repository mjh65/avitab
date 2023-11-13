
var AvitabIsLoaded = false;
document.addEventListener('beforeunload', function () {
    AvitabIsLoaded = false;
}, false);

class AvitabHttp {
    constructor(i,m) {
        this.imageBuffer = i;
        this.navManager = m;
        this.reqId = 1;
        this.lastResponseTime = 0;
        this.lastUpdateTime = 0;
        this.lastPositionTime = 0;
        this.imageLoading = false;
        this.imageReady = false;
        this.imageBuffer.addEventListener("load", () => {
            this.imageReady = true;
            this.imageLoading = false;
            this.lastResponseTime = Date.now();
        });
        this.imageBuffer.addEventListener("error", () => {
            this.imageLoading = false;
            this.lastResponseTime = Date.now();
        });
        this.lastTrafficTime = Date.now() + 5000;
        this.trafficLoading = false;
        this.trafficReady = false;
        let listener = RegisterViewListener('JS_LISTENER_MAPS', () => {
            listener.trigger('JS_BIND_BINGMAP', 'avitab' + Date.now(), false);
        });
    }
    onResponse(resp) {
        //console.log("onResponse " + resp.responseText);
        this.lastResponseTime = Date.now();
        const fields = resp.responseText.split(":");
        if (fields[0] == "NAV") {
            let lat = Number(fields[1]);
            let lon = Number(fields[2]);
            const ca = this.navManager.get();
            if ((ca == null) || (ca.lat != lat) || (ca.lon != lon)) {
                // update the current nav area
                console.log("visitNavArea " + fields[1] + " " + fields[2]);
                this.navManager.visit(lat,lon);
            }
        }
    }
    worldReport(s) {
        var self = this;
        let xhttp = new XMLHttpRequest();
        let url = "http://127.0.0.1:26730/w?t=" + (this.reqId++) + "&nv=" + s;
        xhttp.onreadystatechange = function() {
            if (this.readyState == 4 && this.status == 200) {
                self.onResponse(this);
            }
        };
        xhttp.open("GET", url);
        xhttp.send();
    }
    mouseEvent(x,y,b) {
        var self = this;
        let xhttp = new XMLHttpRequest();
        let url = "http://127.0.0.1:26730/e?t=" + (this.reqId++) + "&mx=" + x + "&my=" + y + "&mb=" + b;
        xhttp.onreadystatechange = function() {
            if (this.readyState == 4 && this.status == 200) {
                self.onResponse(this);
            }
        };
        xhttp.open("GET", url);
        xhttp.send();
    }
    wheelEvent(x,y,d) {
        var self = this;
        let xhttp = new XMLHttpRequest();
        let url = "http://127.0.0.1:26730/e?t=" + (this.reqId++) + "&mx=" + x + "&my=" + y + ((d<0) ? "&wu" : "&wd");
        xhttp.onreadystatechange = function() {
            if (this.readyState == 4 && this.status == 200) {
                self.onResponse(this);
            }
        };
        xhttp.open("GET", url);
        xhttp.send();
    }
    positionUpdate() {
        const lat = SimVar.GetSimVarValue("PLANE LATITUDE", "degree latitude");
        const lon = SimVar.GetSimVarValue("PLANE LONGITUDE", "degree longitude");
        const alt = SimVar.GetSimVarValue("PLANE ALTITUDE", "meters");
        const hdg = SimVar.GetSimVarValue("PLANE HEADING DEGREES TRUE", "degree");
        var self = this;
        let xhttp = new XMLHttpRequest();
        let url = "http://127.0.0.1:26730/u?t=" + (this.reqId++) + "&lt=" + lat.toFixed(4) + "&ln=" + lon.toFixed(4) + "&al=" + alt.toFixed(1) + "&hg=" + hdg.toFixed(0);
        xhttp.onreadystatechange = function() {
            if (this.readyState == 4 && this.status == 200) {
                self.onResponse(this);
            }
        };
        xhttp.open("GET", url);
        xhttp.send();
    }
    trafficUpdate(ta) {
        var self = this;
        let xhttp = new XMLHttpRequest();
        let url = "http://127.0.0.1:26730/u?t=" + (this.reqId++) + "&tr=";
        for (let i = 0; i < ta.length; i++) {
            const entry = ta[i];
            url = url + entry.lat.toFixed(4) + ',' + entry.lon.toFixed(4) + ',' + entry.alt.toFixed(1) + ',' + entry.heading.toFixed(0) + '_';
        }
        xhttp.onreadystatechange = function() {
            if (this.readyState == 4 && this.status == 200) {
                self.onResponse(this);
            }
        };
        xhttp.open("GET", url);
        xhttp.send();
    }
    trafficRequest() {
        this.trafficLoading = true;
        Coherent.call("GET_AIR_TRAFFIC").then((obj) => {
            //console.log("TRAFFIC : " + obj.length + " airplanes found");
            this.trafficUpdate(obj);
            this.trafficLoading = false;
        });
    }
    update() {
        // called on each animation frame callback
        const now = Date.now();

        // get our position every 0.5 seconds
        if (((now - this.lastPositionTime) > 500)) {
            this.lastPositionTime = now;
            this.positionUpdate();
        }

        // get other traffic every 1 second
        if ((this.trafficLoading == false) && ((now - this.lastTrafficTime) > 1000)) {
            this.lastTrafficTime = now;
            this.trafficRequest();
        }

        // may want to throttle this back, currently loads images as fast as responses allow
        if ((this.imageReady == false) && (this.imageLoading == false)) {
            this.imageLoading = true;
            let url = "http://127.0.0.1:26730/f?t=" + (this.reqId++);
            this.imageBuffer.src = url;
        }

        // send any nav data responses that are ready
        let area = this.navManager.get();
        if (area) {
            let node = area.getNextNode();
            if (node) {
                this.worldReport(node);
            }
        }

        // let the caller know if a new image is ready for drawing, or if connection is lost
        let ready = this.imageReady;
        this.imageReady = false;
        return [ready, ((now - this.lastResponseTime) > 3000)];
    }
}

class AvitabElement extends TemplateElement {
    constructor() {
        super(...arguments);
        this.panelActive = false;
        this.ingameUi = null;
        this.borderElement = null;
        this.canvas = null;
        this.canvasX = 800;
        this.canvasY = 480;
        this.imageBuffer = null;
        this.mouseDown = false;
    }
    loadScript(url, callback) {
        var script = document.createElement("script");
        script.type = "text/javascript";
        script.onload = function() { callback(url); };
        script.src = url;
        document.getElementsByTagName("head")[0].appendChild( script );
    }
    loadedCallback(url) {
        console.log('loaded script ' + url);
    }
    connectedCallback() {
        //console.log('connectedCallback()');
        super.connectedCallback();

        var self = this;
        this.ingameUi = this.querySelector('ingame-ui');
        this.borderElement = document.getElementById("AvitabDisplay");
        this.canvas = document.getElementById("AvitabCanvas");
        this.imageBuffer = document.getElementById("ImageBuffer");
        this.navmgr = new NavManager();
        this.http = new AvitabHttp(this.imageBuffer, this.navmgr);

        if (this.ingameUi) {
            this.ingameUi.addEventListener("panelActive", (e) => {
                //console.log('panelActive');
                self.panelActive = true;
                let updateLoop = () => {
                    if (window["IsDestroying"] === true) {
                        return;
                    }
                    if (!AvitabIsLoaded) {
                        return;
                    }
                    if (typeof window.AvitabNodesIsLoaded == 'undefined') {
                        console.log('AvitabNodes.js not yet loaded');
                    }
                    if (typeof window.AvitabWorldIsLoaded == 'undefined') {
                        console.log('AvitabWorld.js not yet loaded');
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
                if (this.mouseDown) {
                    this.http.mouseEvent(Math.round(800 * e.offsetX / this.canvasX), Math.round(480 * e.offsetY / this.canvasY), 1);
                }
            });
            this.canvas.addEventListener("mousedown", (e) => {
                this.mouseDown = true;
                this.http.mouseEvent(Math.round(800 * e.offsetX / this.canvasX), Math.round(480 * e.offsetY / this.canvasY), 1);
            });
            this.canvas.addEventListener("mouseup", (e) => {
                this.mouseDown = false;
                this.http.mouseEvent(Math.round(800 * e.offsetX / this.canvasX), Math.round(480 * e.offsetY / this.canvasY), 0);
            });
            this.canvas.addEventListener("wheel", (e) => {
                this.http.wheelEvent(Math.round(800 * e.offsetX / this.canvasX), Math.round(480 * e.offsetY / this.canvasY), e.deltaY);
            });
        }
    }
    disconnectedCallback() {
        super.disconnectedCallback();
    }
    onPanelResized() {
        let scale = this.borderElement.offsetWidth / 800;
        if ((this.borderElement.offsetHeight / 480) < scale) {
            scale = this.borderElement.offsetHeight / 480;
        }
        this.canvasX = Math.round(800 * scale);
        this.canvasY = Math.round(480 * scale);
        this.canvas.width = this.canvasX;
        this.canvas.height = this.canvasY;
    }
    updateCanvas() {
        let ctx = this.canvas.getContext("2d");
        ctx.drawImage(this.imageBuffer, 0, 0, 800, 480, 0, 0, this.canvasX, this.canvasY);
    }
    noConnection(me) {
        // TODO - scale this to fit canvas
        let ctx = this.canvas.getContext("2d");
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
    }
    flightLoop() {
        let res = this.http.update();
        if (res[1]) {
            this.noConnection();
        } else if (res[0]) {
            this.updateCanvas();
        }
    }
}
window.customElements.define("aviators-tablet", AvitabElement);
checkAutoload();
