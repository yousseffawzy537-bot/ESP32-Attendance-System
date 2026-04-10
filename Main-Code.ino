var ss = SpreadsheetApp.openById('1xniR72rjtdbCQDV9xYazklfja9otQrck-LYvFOVMPwI');
var sheet = ss.getSheetByName('Sheet1');
var timezone = "Eastern European Time/Cairo";

function doGet(e){
  Logger.log( JSON.stringify(e) );
  if (e.parameter == 'undefined') {
    return ContentService.createTextOutput("Received data is undefined");
  }



  var Curr_Date = new Date();
  var Curr_Time = Utilities.formatDate(Curr_Date, timezone, 'HH:mm:ss');
  var name = stripQuotes(e.parameters.name);
  var nextRow = sheet.getLastRow() + 1;
  let data = sheet.getRange("A:A").getValues().flat();
  let check = data.includes(name) ? "+" : "-";
  if (check == "-"){
    sheet.getRange("A" + nextRow).setValue(name);
    sheet.getRange("B" + nextRow).setValue(Curr_Date);
    sheet.getRange("C" + nextRow).setValue(Curr_Time);
    let names =["Fawzy","Ahmed"];
    let found = "false";
    for(let i = 0; i < names.length; i++) {
      if(name === names[i]){
        found = "true";
        break;
      }
    }
    let access;

    if (found == "true"){
      access = "accepted";
    }
    else {
      access = "denied";
    }
    sheet.getRange("D" + nextRow).setValue(access);

    function timeToMinutes(time) {
      let [h, m, s] = time.split(":").map(Number);
      return h * 60 * 60 + m * 60 + s;
    }
    
    if (access == "accepted") {
      let lec_time = "22:44:00";
      if(timeToMinutes(lec_time) >= timeToMinutes(Curr_Time)) {
        sheet.getRange("E" + nextRow).setValue("Present");
      }else{
        sheet.getRange("E" + nextRow).setValue("Late");
      }
    }

    return ContentService.createTextOutput("Card holder name is stored in column A");
  }


}


function stripQuotes( value ) {
  return value.toString().replace(/^["']|['"]$/g, "");
}

function doPost(e) {
  var val = e.parameter.value;
  
  if (e.parameter.value !== undefined){
    var range = sheet.getRange('A2');
    range.setValue(val);
  }
}