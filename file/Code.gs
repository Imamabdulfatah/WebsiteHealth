// Enter Spreadsheet ID here
var SS = SpreadsheetApp.openById('1IWWpruJ2gHDjm9bmTqwDDL2_sKqt90HTYM3BRKsq7hw');
var str = "";
const sheet = SS.getSheetByName("Sheet1")


function doGet(e) {
  let obj = {}
  let data = sheet.getDataRange().getValues();
  obj.content = data;
  return ContentService.createTextOutput(JSON.stringify(obj)).setMimeType(ContentService.MimeType.JSON)
  
}


function doPost(e) {
  
  var parsedData;
  var result = {};
  
  try { 
    parsedData = JSON.parse(e.postData.contents);
  } 
  catch(f){
    return ContentService.createTextOutput("Error in parsing request body: " + f.message);
  }
   
  if (parsedData !== undefined){
    var flag = parsedData.format;
    if (flag === undefined){
      flag = 0;
    }
    
    const sheet = SS.getSheetByName(parsedData.Sheet1); // sheet name to publish data to is specified in Arduino code
    var dataArr = parsedData.values.split(","); // creates an array of the values to publish 

    // Default time zone is America/Chicago. Update time zone below with appropriate ID from here: https://developers.google.com/google-ads/api/data/codes-formats#timezone-ids
    var date_now = Utilities.formatDate(new Date(), "Asia/Jakarta", "yyyy/MM/dd"); // gets the current date
    var time_now = Utilities.formatDate(new Date(), "Asia/Jakarta", "hh:mm:ss a"); // gets the current time
    
    var value0 = dataArr [0]; // value0 from Arduino code
   
    // read and execute command from the "payload_base" string specified in Arduino code
    switch (parsedData.command) {
      
      case "insert_row":
         
         sheet.insertRows(2); // insert full row directly below header text
         
         //var range = sheet.getRange("A2:D2");              // use this to insert cells just above the existing data instead of inserting an entire row
         //range.insertCells(SpreadsheetApp.Dimension.ROWS); // use this to insert cells just above the existing data instead of inserting an entire row
         
         sheet.getRange('A2').setValue(date_now); // publish current date to cell A2
         sheet.getRange('B2').setValue(time_now); // publish current time to cell B2
         sheet.getRange('C2').setValue(detak_jantung);   // publish value0 from Arduino code to cell C2
        
         str = "Success"; // string to return back to Arduino serial console
         SpreadsheetApp.flush();
         break;
         
      case "append_row":
         
         var publish_array = new Array(); // create a new array
         
         publish_array [0] = date_now; // add current date to position 0 in publish_array
         publish_array [1] = time_now; // add current time to position 1 in publish_array
         publish_array [2] = detak_jantung;   // add value0 from Arduino code to position 2 in publish_array
         
         
         sheet.appendRow(publish_array); // publish data in publish_array after the last row of data in the sheet
         
         str = "Success"; // string to return back to Arduino serial console
         SpreadsheetApp.flush();
         break;     
 
    }
    
    return ContentService.createTextOutput(str);
  } // endif (parsedData !== undefined)
  
  else {
    return ContentService.createTextOutput("Error! Request body empty or in incorrect format.");
  }
}