
var ip_server = "ws://" + location.host + "/serial_k210";

console.log(ip_server);
var socket1 = new WebSocket(ip_server);
var from_b64_str = "";
socket1.onmessage = function (event) {
    if (event.data.indexOf("os.listdir") >= 0) {

        console.log(event.data);
        let text = event.data.substring(event.data.indexOf("[") + 1, event.data.indexOf("]"));
        let txt_js = "\"" + text + "\"";
        console.log(text);
        console.log(txt_js);
        let njson = text.split(",");
        njson.forEach((value) => {
            let t = value.split("'");
            console.log(t);
            if (value.indexOf(".") >= 0) {

                addFile(t[1]);
            }
            else {

                addFolder(root_folder + "/" + t[1]);
            }

        });
    }
    // else if (event.data.indexOf("#64") >= 0) {
    //     // from_b64_to_blob(event.data);
    //     from_b64_str += event.data;

    // }
    else if (event.data.indexOf("endFile") >= 0) {
        from_b64_str += event.data;
        console.log("endFile");
        console.log(from_b64_str);
        from_b64_to_blob(from_b64_str);
        from_b64_str = "";

    }
    else {
        from_b64_str += event.data;
        console.log(from_b64_str);

    }
    // else if (event.data.indexOf("file_name") >= 0) {
    //     from_b64_to_blob(event.data);
    // }
}
var root_folder = ""
var dwl_file_name = ""
function list_file_fun() {
    let e = document.getElementById("folders");
    let text = e.options[e.selectedIndex].text;
    root_folder = text;
    socket1.send("lf[" + text + "]");
}

function parse_64base_string(b64string) {

    let full_str = "";
    let SUB_str = "";
    let full_st_arr;
    let startStr = b64string.indexOf("#64");
    let endStr = b64string.indexOf("endFile");
    if (endStr >= 0) {
        SUB_str = b64string.substring(startStr, endStr);
        full_st_arr = SUB_str.split("#64");
        full_st_arr.forEach((e) => {
            full_str += e;
        });
        full_st_arr = full_str.split("#");
        console.log(full_st_arr);
        full_str = full_st_arr[0] + full_st_arr[1].substring(2, full_st_arr[1].length)
        console.log(full_str);
    }
    return full_str;
}

function from_b64_to_blob(b64string) {
    console.log(b64string);
    let decode_str = atob(parse_64base_string(b64string));
    download(dwl_file_name, decode_str);
    return decode_str;
}



function addFile(value) {
    var x = document.getElementById("dirListOpt");
    var option = document.createElement("option");
    option.text = value;
    x.add(option, x[0]);
}

function addFolder(value) {
    var x = document.getElementById("folders");
    var option = document.createElement("option");
    option.text = value;
    x.add(option, x[0]);
}

function dowload_file_fun() {
    let e = document.getElementById("dirListOpt");
    let text = e.options[e.selectedIndex].text;
    dwl_file_name = text;
    console.log(dwl_file_name);
    socket1.send("dwl[" + text + "]");
}
//   function addList(value){
//     var select = document.getElementById("folders");
//     for(var i = 2011; i >= 1900; --i) {
//     var option = document.createElement('option');
//     option.text = option.value = value;
//     select.add(option, 0);
//       }
//      }


function saveB64File(textFileAsBlob, fileNameToSaveAs) {
    const link = document.createElement('a')
    const url = URL.createObjectURL(textFileAsBlob)

    link.href = url
    link.download = fileNameToSaveAs
    document.body.appendChild(link)
    link.click()

    document.body.removeChild(link)
    window.URL.revokeObjectURL(url)
}

function download(filename, text) {
    var element = document.createElement('a');
    element.setAttribute('href', 'data:text/plain;charset=utf-8,' + encodeURIComponent(text));
    element.setAttribute('download', filename);

    element.style.display = 'none';
    document.body.appendChild(element);

    element.click();

    document.body.removeChild(element);
}
