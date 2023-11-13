
var AvitabNodesIsLoaded = true;

// Much of this is copied from workingtitle-ingamepanels-vfrmap/html_ui/InGamePanels/GameVFRMap/GameVFRMap.js

// NodeLoader is a reduced version of FacilityLoader (lines 7232-)
// The class seems to be responsible for getting the facility reports from the Coherent searches
// Our version doesn't have a local repo (cache) and
class NodeLoader {
    constructor(onInitialized = () => { }) {
        this.onInitialized = onInitialized;
        if (NodeLoader.facilityListener === undefined) {
            NodeLoader.facilityListener = RegisterViewListener('JS_LISTENER_FACILITY', () => {
                NodeLoader.facilityListener.on('SendAirport', NodeLoader.onFacilityReceived);
                NodeLoader.facilityListener.on('SendIntersection', NodeLoader.onFacilityReceived);
                NodeLoader.facilityListener.on('SendVor', NodeLoader.onFacilityReceived);
                NodeLoader.facilityListener.on('SendNdb', NodeLoader.onFacilityReceived);
                NodeLoader.facilityListener.on('NearestSearchCompleted', NodeLoader.onNearestSearchCompleted);
                setTimeout(() => NodeLoader.init(), 5000);
            }, true);
        }
        this.awaitInitialization().then(() => this.onInitialized());
    }
    static init() {
        console.log('NodeLoader::init()');
        NodeLoader.isInitialized = true;
        for (const resolve of this.initPromiseResolveQueue) {
            resolve();
        }
        this.initPromiseResolveQueue.length = 0;
    }
    awaitInitialization() {
        console.log('NodeLoader::awaitInitialization()');
        if (NodeLoader.isInitialized) {
            return Promise.resolve();
        }
        else {
            return new Promise(resolve => {
                NodeLoader.initPromiseResolveQueue.push(resolve);
            });
        }
    }
    async startNearestSearchSession() {
        if (!NodeLoader.isInitialized) {
            await this.awaitInitialization();
        }
        const sessionId = await Coherent.call('START_NEAREST_SEARCH_SESSION', 0); // 0 is all facilities
        let session = new CoherentNearestSearchSession(sessionId);
        NodeLoader.searchSessions.set(sessionId, session);
        return session;
    }
    static onFacilityReceived(facility) {
        console.log('onFacilityReceived()');
        const isMismatch = facility['__Type'] === 'JS_FacilityIntersection' && facility.icao[0] !== 'W';
        const queue = isMismatch ? NodeLoader.mismatchRequestQueue : NodeLoader.requestQueue;
        const request = queue.get(facility.icao);
        if (request !== undefined) {
            request.resolve(facility);
            NodeLoader.addToFacilityCache(facility, isMismatch);
            queue.delete(facility.icao);
        }
    }
    static onNearestSearchCompleted(results) {
        console.log('onNearestSearchCompleted()');
        const session = NodeLoader.searchSessions.get(results.sessionId);
        if (session instanceof CoherentNearestSearchSession) {
            session.onSearchCompleted(results);
        }
    }
}
NodeLoader.requestQueue = new Map();
NodeLoader.mismatchRequestQueue = new Map();
//NodeLoader.facCache = new Map();
//NodeLoader.typeMismatchFacCache = new Map();
//NodeLoader.airwayCache = new Map();
NodeLoader.searchSessions = new Map();
//NodeLoader.facRepositorySearchTypes = {
//    [FacilitySearchType.All]: [FacilityType.USR, FacilityType.VIS],
//    [FacilitySearchType.User]: [FacilityType.USR],
//    [FacilitySearchType.Visual]: [FacilityType.VIS],
//    [FacilitySearchType.AllExceptVisual]: [FacilityType.USR]
//};
//NodeLoader.repoSearchSessionId = -1;
NodeLoader.isInitialized = false;
NodeLoader.initPromiseResolveQueue = [];


class CoherentNearestSearchSession {
    constructor(sessionId) {
        this.sessionId = sessionId;
        this.searchQueue = new Map();
    }
    searchNearest(lat, lon, radius, maxItems) {
        const promise = new Promise((resolve) => {
            let searchPromise = Coherent.call('SEARCH_NEAREST', this.sessionId, lat, lon, radius, maxItems);
            searchPromise.then((searchId) => {
                console.log('searchPromise callback' + searchId);
                this.searchQueue.set(searchId, { promise, resolve });
            });
            searchPromise.catch((error) => {
                console.error(error);
            });
        });
        return promise;
    }
    onSearchCompleted(results) {
        console.log('onSearchCompleted');
        const request = this.searchQueue.get(results.searchId);
        if (request !== undefined) {
            request.resolve(results);
            this.searchQueue.delete(results.searchId);
        }
    }
}

