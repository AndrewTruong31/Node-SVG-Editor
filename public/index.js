// Put all onload AJAX calls here, and event listeners
$(document).ready(function() {
    // On page-load AJAX Example
    var visible = document.getElementById("svgViewPanel");
    var displayTableData2 = document.getElementById("tableData2");
    var displayTableData3 = document.getElementById("tableData3");
    var displayTableData4 = document.getElementById("tableData4");
    displayTableData2.style.display = "none";
    displayTableData3.style.display = "none";
    displayTableData4.style.display = "none";
    visible.style.display = "none";
    document.getElementById('editName').value = null;
    document.getElementById('editValue').value = null;
    document.getElementById('addName').value = null;
    document.getElementById('addValue').value = null;
    document.getElementById('changeTitle').value = null;
    document.getElementById('changeDesc').value = null;
    $.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything 
        url: '/someendpoint',   //The server endpoint we are connecting to
        data: {
            name1: "Value 1",
            name2: "Value 2"
        },
        success: function (data) {
            /*  Do something with returned object
                Note that what we get is an object, not a string, 
                so we do not need to parse it on the server.
                JavaScript really does handle JSONs seamlessly
            */
            $('#blah').html("On page load, received string '"+data.foo+"' from server");
            //We write the object to the console to show that the request was successful
            console.log(data); 

        },
        fail: function(error) {
            // Non-200 return, do something with error
            $('#blah').html("On page load, received error from server");
            console.log(error); 
        }
    });

    $.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything 
        url: '/showuploads',   //The server endpoint we are connecting to
        data: {
            name: "uploads/",
        },
        success: function (data) {
            var table = document.getElementById("flogTable");
            var select = document.getElementById("viewPanelSelect");

            for(let i = 0; i < data.arr.length; i ++){
                var row = table.insertRow(table.rows.length);
                var cell1 = row.insertCell(0);
                var cell2 = row.insertCell(1);
                var cell3 = row.insertCell(2);
                var cell4 = row.insertCell(3);
                var cell5 = row.insertCell(4);
                var cell6 = row.insertCell(5);
                var cell7 = row.insertCell(6);
                cell1.innerHTML = '<a href=\"uploads/' + data.names[i] + '\" download>' + '<img src=\"' + data.names[i] + '\" width="200">' + '</a>';
                cell2.innerHTML = '<a href=\"uploads/' + data.names[i] + '\" download>' + data.names[i] + '</a>';
                cell3.innerHTML = data.size[i] + 'KB';
                cell4.innerHTML = data.arr[i].numRect;
                cell5.innerHTML = data.arr[i].numCirc;
                cell6.innerHTML = data.arr[i].numPaths;
                cell7.innerHTML = data.arr[i].numGroups;

                var option = document.createElement("OPTION");
                txt = document.createTextNode(data.names[i]);
                option.appendChild(txt);
                option.setAttribute('value', data.names[i]);
                select.insertBefore(option, select.lastChild);
                select.selectedIndex = "-1";
            }
        },
        fail: function(error) {
            console.log(error); 
        }
    });

    // Event listener form example , we can use this instead explicitly listening for events
    let nameList = [];
    let valList = [];

    document.getElementById('addSubmit').onclick = function(){
        let file = 'uploads/'
        file = file + document.getElementById('viewPanelSelect').value;
        let shape = document.getElementById('addSelect').value;
        let info = true;
        let len = nameList.length;
        if(shape == 'Rectangle'){
            for(let i = 0; i < nameList.length; i ++){ //rectangle
                if(nameList.indexOf('x') < 0) info = false;
                if(nameList.indexOf('y') < 0) info = false;
                if(nameList.indexOf('width') < 0) info = false;
                if(nameList.indexOf('height') < 0) info = false;
            }
        }
            
        if(shape == 'Circle'){
            for(let i = 0; i < nameList.length; i ++){ //circle
                if(nameList.indexOf('cx') < 0) info = false;
                if(nameList.indexOf('cy') < 0) info = false;
                if(nameList.indexOf('r') < 0) info = false;
            }
        }
            
        if(shape == 'Path'){
            for(let i = 0; i < nameList.length; i ++){ //path
                if(nameList.indexOf('d') < 0) info = false;
            }
        }

        if(info == true && file != null){
            if(document.getElementById('viewPanelSelect').value.length > 0){
                $.ajax({
                    type: 'get',            //Request type
                    dataType: 'json',       //Data type - we will use JSON for almost everything 
                    url: '/addComp',   //The server endpoint we are connecting to
                    data: {
                        nameList,
                        valList,
                        file,
                        shape,
                        len
                    },
                    success: function (data) {
                        if(data.validate == 0){
                            alert('There was an invalid argument');
                        }
            
                    },
                    fail: function(error) {
                        console.log(error); 
                    }
                });
                nameList = [];
                valList = [];
                $('#editTableInner td').remove();
            }else{
                alert('No Image is Selected');
            }
        }else{
            alert('Shape is Missing Information');
        }
    }

    

    document.getElementById('addAttr').onclick = function(){
        var table = document.getElementById('editTableInner');
        namebtn = document.getElementById('addName');
        valbtn = document.getElementById('addValue');

        if(namebtn.value != '' && valbtn.value != ''){
            nameList[nameList.length] = namebtn.value;
            valList[valList.length] = valbtn.value;

            var row = table.insertRow(table.rows.length);
            var cell1 = row.insertCell(0);
            var cell2 = row.insertCell(1);

            cell1.innerHTML = namebtn.value;
            cell2.innerHTML = valbtn.value;        

        }else{
            alert('Name or Value is invalid');
        }

        document.getElementById('addName').value = null;
        document.getElementById('addValue').value = null;
    }

    document.getElementById('changeDescBtn').onclick = function(){
        let desc = document.getElementById('changeDesc').value;
        let file = 'uploads/'
        file = file + document.getElementById('viewPanelSelect').value;
        if(desc.length > 0){
            $.ajax({
                type: 'get',            //Request type
                dataType: 'json',       //Data type - we will use JSON for almost everything 
                url: '/setdesc',   //The server endpoint we are connecting to
                data: {
                    desc,
                    file
                },
            });
            document.getElementById('changeDesc').value = null;
        }else alert('Please input a value');
    }

    document.getElementById('changeTitBtn').onclick = function(){
        let title = document.getElementById('changeTitle').value;
        let file = 'uploads/'
        file = file + document.getElementById('viewPanelSelect').value;
        if(title.length > 0){
            $.ajax({
                type: 'get',            //Request type
                dataType: 'json',       //Data type - we will use JSON for almost everything 
                url: '/settit',   //The server endpoint we are connecting to
                data: {
                    title,
                    file
                },
            });
            document.getElementById('changeTitle').value = null;
        }else alert('Please input a value');
    }

    document.getElementById('editValueButton').onclick = function(){
        displayTableData2.style.display = "block";
        displayTableData3.style.display = "none";
        displayTableData4.style.display = "none";
    }

    document.getElementById('editTitDec').onclick = function(){
        displayTableData2.style.display = "none";
        displayTableData3.style.display = "none";
        displayTableData4.style.display = "block";
    }

    document.getElementById('addCompButton').onclick = function(){
        if(document.getElementById('viewPanelSelect').value.length > 0){
            nameList = [];
            valList = [];

            displayTableData2.style.display = "none";
            displayTableData3.style.display = "block";
            displayTableData4.style.display = "none";
            document.getElementById('showAttrSelect').selectedIndex = -1;
            $('#editTableInner td').remove();
        }else alert('Please select a Image');
        
    }

    document.getElementById('editSubmit').onclick = function(){
        let name = document.getElementById('editName').value;
        let value = document.getElementById('editValue').value;
        
        var file = document.getElementById('showAttrSelect').value;
        var info = file.split(' ');

        $.ajax({
            type: 'get',            //Request type
            dataType: 'json',       //Data type - we will use JSON for almost everything 
            url: '/editValue',   //The server endpoint we are connecting to
            data: {
                name,
                value,
                info
            },
            success: function (data) {
                
                console.log(data); 
            },
            fail: function(error) {
                console.log(error); 
            }
        });
    }

    document.getElementById('showAttrSelect').onchange = function(){
        var file = document.getElementById('showAttrSelect').value;
        var table = document.getElementById('editTableInner');
        var info = file.split(' ');
        var arr = [];
        displayTableData2.style.display = "none";
        displayTableData3.style.display = "none";
        displayTableData4.style.display = "none";
        $('#editTableInner td').remove();
        $.ajax({
            type: 'get',            //Request type
            dataType: 'json',       //Data type - we will use JSON for almost everything 
            url: '/getAttr',   //The server endpoint we are connecting to
            data: {
                info
            },
            success: function (data) {
                //We write the object to the console to show that the request was successful         
                arr = JSON.parse(data.json);
            
                let len = arr.length;
                
                for(let i = 0; i < len; i ++){
                    var row = table.insertRow(table.rows.length);
                    var cell1 = row.insertCell(0);
                    var cell2 = row.insertCell(1);

                    cell1.innerHTML = arr[i].name;
                    cell2.innerHTML = arr[i].value.substring(0,64);
                    
                }
    
            },
            fail: function(error) {
                console.log(error); 
            }
        });
    }

    document.getElementById('viewPanelSelect').onchange = function(){
        visible.style.display = "block";
        displayTableData2.style.display = "none";
        displayTableData3.style.display = "none";
        displayTableData4.style.display = "none";
        var select = document.getElementById('viewPanelSelect');
        var showAttr = document.getElementById('showAttrSelect');
        $('#editTableInner td').remove();
        var length = showAttr.options.length;
        for (i = length-1; i >= 0; i--) {
            showAttr.options[i] = null;
        }
        $.ajax({
            type: 'get',            //Request type
            dataType: 'json',       //Data type - we will use JSON for almost everything 
            url: '/selectsvg',   //The server endpoint we are connecting to
            data: {
                name: select.value,
            },
            success: function (data) {
                document.getElementById("svgViewPanelImg").src = data.name;
                document.getElementById("svgViewTitle").innerHTML = data.title;
                document.getElementById("svgViewDesc").innerHTML = data.desc;

                $('#svgViewTable2 td').remove();
                var table = document.getElementById("svgViewTable2");

                for(let i = 0; i < data.rectJSON.length; i ++){
                    var row = table.insertRow(table.rows.length);
                    var cell1 = row.insertCell(0);
                    var cell2 = row.insertCell(1);
                    var cell3 = row.insertCell(2);

                    cell1.innerHTML = 'Rectangle ' + i;
                    cell2.innerHTML = 'Upper left Corner: x = ' + data.rectJSON[i].x + data.rectJSON[i].units + ', y ='
                        + data.rectJSON[i].y + data.rectJSON[i].units + ', Width: ' + data.rectJSON[i].w + data.rectJSON[i].units + ', Height = '
                        + data.rectJSON[i].h + data.rectJSON[i].units;
                    cell3.innerHTML = data.rectJSON[i].numAttr;

                    var option = document.createElement("OPTION");
                    txt = document.createTextNode('Rectangle ' + i);
                    option.appendChild(txt);
                    option.setAttribute('value', data.name + ' Rectangle ' + i);
                    showAttr.insertBefore(option, showAttr.lastChild);
                }

                for(let i = 0; i < data.circJSON.length; i ++){
                    var row = table.insertRow(table.rows.length);
                    var cell1 = row.insertCell(0);
                    var cell2 = row.insertCell(1);
                    var cell3 = row.insertCell(2);            
                    
                    cell1.innerHTML = 'Circle ' + i;
                    cell2.innerHTML = 'Center: x = ' + data.circJSON[i].cx + data.circJSON[i].units + ', y = '
                        + data.circJSON[i].cy + data.circJSON[i].units + ', radius = ' + data.circJSON[i].r + data.circJSON[i].units;
                    cell3.innerHTML = data.circJSON[i].numAttr;

                    var option = document.createElement("OPTION");
                    txt = document.createTextNode('Circle ' + i);
                    option.appendChild(txt);
                    option.setAttribute('value', data.name + ' Circle ' + i);
                    showAttr.insertBefore(option, showAttr.lastChild);
                }

                for(let i = 0; i < data.pathJSON.length; i ++){
                    var row = table.insertRow(table.rows.length);
                    var cell1 = row.insertCell(0);
                    var cell2 = row.insertCell(1);
                    var cell3 = row.insertCell(2);           
                    
                    cell1.innerHTML = 'Path ' + i;
                    cell2.innerHTML = 'Path data = ' + data.pathJSON[i].d;
                    cell3.innerHTML = data.pathJSON[i].numAttr;

                    var option = document.createElement("OPTION");
                    txt = document.createTextNode('Path ' + i);
                    option.appendChild(txt);
                    option.setAttribute('value', data.name + ' Path ' + i);
                    showAttr.insertBefore(option, showAttr.lastChild);
                }

                for(let i = 0; i < data.groupJSON.length; i ++){
                    var row = table.insertRow(table.rows.length);
                    var cell1 = row.insertCell(0);
                    var cell2 = row.insertCell(1);
                    var cell3 = row.insertCell(2);               
                    
                    cell1.innerHTML = 'Group ' + i;
                    cell2.innerHTML = data.groupJSON[i].children + ' child elements';
                    cell3.innerHTML = data.groupJSON[i].numAttr;

                    var option = document.createElement("OPTION");
                    txt = document.createTextNode('Group ' + i);
                    option.appendChild(txt);
                    option.setAttribute('value', data.name + ' Group ' + i);
                    showAttr.insertBefore(option, showAttr.lastChild);
                }

                showAttr.selectedIndex = '-1';

            },
            fail: function(error) {
                console.log(error); 
            }
        });
    }

    // No redirects if possible
    $('#someform').submit(function(e){
        $('#blah').html("Form has data: "+$('#entryBox').val());
        e.preventDefault();
        //Pass data to the Ajax call, so it gets passed to the server
        $.ajax({
            //Create an object for connecting to another waypoint
        });
    });
});