
var AvitabWorldIsLoaded = true;

// Much of this is copied from workingtitle-ingamepanels-vfrmap/html_ui/InGamePanels/GameVFRMap/GameVFRMap.js

class NavArea {
    constructor(y,x) {
        this.lat = y;
        this.lon = x;
    }
    getNextNode() {
        return null;
    }
}

class NavManager {
    constructor() {
        this.currentArea = null;
        this.nodeLoader = new NodeLoader(this.onNodeLoaderInitialized.bind(this));
        this.coherentSearchSession = null;
    }
    onNodeLoaderInitialized() {
        Promise.all([
            this.nodeLoader.startNearestSearchSession(),
        ]).then((value) => {
            const [searchSession] = value;
            this.onSessionStarted(searchSession);
        });
    }
    onSessionStarted(session) {
        console.log('onSessionStarted()')
        this.coherentSearchSession = session;
    }
    async visit(lat,lon) {
        if (this.coherentSearchSession) {
            console.log('NavManager::visit('+lat+','+lon+')');
            this.currentArea = new NavArea(lat,lon);
            console.log('NavManager::visit - before await coherentSearchSession');
            const results = await this.coherentSearchSession.searchNearest(lat,lon,1000,20);
            console.log('NavManager::visit - after await coherentSearchSession');
            console.log(results);
        }
    }
    get() {
        return this.currentArea;
    }
    next() {
        if (this.currentArea) {
            let report = "Z" + this.currentArea.lat + "," + this.currentArea.lon;
            this.currentArea = null;
            return report;
        }
        return "Q";
    }
}
