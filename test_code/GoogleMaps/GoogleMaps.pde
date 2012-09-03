import com.francisli.processing.http.*;

double longitude;
double latitude;

boolean responseProcessed = false;

void setup() {
  // initialise to -1
  longitude = -1;
  latitude = -1;
  
  getCoordinatesFromString("E9+7HD"); 
}


// -------------------------------------------------------------
// getCoordinatesFromString: given a string-based post-code it queries
// Google Maps API for the longitude and latitude. Response is handled
// by the responseReceived function
void getCoordinatesFromString(String location)
{
  // create new client
  HttpClient client = new HttpClient(this, "maps.googleapis.com");

  // instantiate a new HashMap
  HashMap params = new HashMap();

  // pass parameters as key-value pairs
  params.put("address", location);
  params.put("sensor", "true");

  // make the request
  client.GET("/maps/api/geocode/json", params);
}

// -------------------------------------------------------------
// responseReceived: implemented here to handle responses from
// Google API so that the latitude and longitude of a response
// are stored in global varialbles 'longitude' and 'latitude'
void responseReceived(HttpRequest request, HttpResponse response) 
{
  // check for HTTP 200 success code
  if (response.statusCode == 200) {
    JSONObject results = response.getContentAsJSONObject();

    longitude = results.get("results").get(0).get("geometry").get("location").get("lng").floatValue();
    latitude = results.get("results").get(0).get("geometry").get("location").get("lat").floatValue();
       
    printLongitudeAndLatitude();
     
  } 
  else {
    // output the entire response as a string
    println(response.getContentAsString());
  }
}

// -------------------------------------------------------------
// printLongitudeAndLatitude: print out co-ordinates
void printLongitudeAndLatitude()
{
   println(latitude + ", " + longitude); 
}

void draw() {
}

