'use strict'

// C library API
const ffi = require('ffi-napi');

// Express App (Routes)
const express = require("express");
const app     = express();
const path    = require("path");
const fileUpload = require('express-fileupload');

app.use(fileUpload());
app.use(express.static(path.join(__dirname+'/uploads')));

// Minimization
const fs = require('fs');
const JavaScriptObfuscator = require('javascript-obfuscator');

// Important, pass in port as in `npm run dev 1234`, do not change
const portNum = process.argv[2];

// Send HTML at root, do not change
app.get('/',function(req,res){
  res.sendFile(path.join(__dirname+'/public/index.html'));
});

// Send Style, do not change
app.get('/style.css',function(req,res){
  //Feel free to change the contents of style.css to prettify your Web app
  res.sendFile(path.join(__dirname+'/public/style.css'));
});

// Send obfuscated JS, do not change
app.get('/index.js',function(req,res){
  fs.readFile(path.join(__dirname+'/public/index.js'), 'utf8', function(err, contents) {
    const minimizedContents = JavaScriptObfuscator.obfuscate(contents, {compact: true, controlFlowFlattening: true});
    res.contentType('application/javascript');
    res.send(minimizedContents._obfuscatedCode);
  });
});

//Respond to POST requests that upload files to uploads/ directory
app.post('/upload', function(req, res) {
  if(!req.files) {
    return res.status(400).send('No files were uploaded.');
  }
 
  let uploadFile = req.files.uploadFile;
 
  // Use the mv() method to place the file somewhere on your server
  uploadFile.mv('uploads/' + uploadFile.name, function(err) {
    if(err) {
      return res.status(500).send(err);
    }

    res.redirect('/');
  });
});

//Respond to GET requests for files in the uploads/ directory
app.get('/uploads/:name', function(req , res){
  fs.stat('uploads/' + req.params.name, function(err, stat) {
    if(err == null) {
      res.sendFile(path.join(__dirname+'/uploads/' + req.params.name));
    } else {
      console.log('Error in file downloading route: '+err);
      res.send('');
    }
  });
});

//******************** Your code goes here ******************** 

//Import C library
let sharedLib = ffi.Library('./libsvgparse.so',{
  'interpretFileCall': ['string', ['string', 'int']],
  'getFileSize': ['int', ['string']],
  'getTitle': ['string', ['string']],
  'getDesc': ['string', ['string']],
  'getAttr': ['string', ['string', 'string', 'int']],
  'changeAttr': ['int', ['string', 'string', 'int', 'string', 'string']],
  'addCompJs': ['int', ['string', 'string', 'string', 'string', 'int']],
  'setTitleJs': ['void', ['string', 'string']],
  'setDescJs': ['void', ['string', 'string']]
});

//Sample endpoint
app.get('/someendpoint', function(req , res){
  // let retStr = req.query.name1 + " " + req.query.name2;
  // res.send({
  //   foo: retStr
  // });
});

app.get('/addComp', function(req , res){
  let file = req.query.file;
  let nameList = req.query.nameList;
  let valList = req.query.valList;
  let shape = req.query.shape;
  let len = req.query.len;
  let name = nameList[0];
  for(let i = 1; i < len; i ++){
    name = name + ' ' + nameList[i];
  }
  let value = valList[0];
  for(let i = 1; i < len; i ++){
    value = value + ' ' + valList[i];
  }

  let validate = sharedLib.addCompJs(file, shape, name, value, len);

  res.send({
    validate
  });
});

app.get('/editValue', function(req , res){
  let name = req.query.name;
  let value = req.query.value;
  let file = req.query.info[0];
  let type = req.query.info[1];
  let index = req.query.info[2];
  let ret = sharedLib.changeAttr(file, type, index, name, value);
  res.send({
    ret
  });
});

app.get('/getAttr', function(req , res){
  let file = req.query.info[0];
  let type = req.query.info[1];
  let index = req.query.info[2];
  let json = sharedLib.getAttr(file, type, index);
  res.send({
    json
  });
});

app.get('/setdesc', function(req , res){
  let desc = req.query.desc;
  let file = req.query.file;
  sharedLib.setDescJs(file, desc);
});

app.get('/settit', function(req , res){
  let title = req.query.title;
  let file = req.query.file;
  sharedLib.setTitleJs(file, title);
});


app.get('/selectsvg', function(req , res){
  let name = 'uploads/' + req.query.name;
  let title = sharedLib.getTitle('uploads/' + req.query.name);
  let desc = sharedLib.getDesc('uploads/' + req.query.name);
  let circList = sharedLib.interpretFileCall('uploads/' + req.query.name, 2)
  let circJSON = JSON.parse(circList);
  let rectList = sharedLib.interpretFileCall('uploads/' + req.query.name, 3);
  let rectJSON = JSON.parse(rectList);
  let pathList = sharedLib.interpretFileCall('uploads/' + req.query.name, 4);
  let pathJSON = JSON.parse(pathList);
  let groupList = sharedLib.interpretFileCall('uploads/' + req.query.name, 5);
  let groupJSON = JSON.parse(groupList);
  res.send({
    name,
    title, 
    desc, 
    pathJSON,
    rectJSON,
    circJSON,
    groupJSON
  });

});

app.get('/showuploads', function(req , res){
  const dirname = 'uploads/';
  var arr = [];
  var names = [];
  var size = [];
  let i = 0;
  fs.readdirSync(dirname).forEach(file => {

      let retStr = sharedLib.interpretFileCall(req.query.name+file,1);
      let json = JSON.parse(retStr);
      size[i] = sharedLib.getFileSize(req.query.name+file);
      arr[i] = json;
      names[i] = file;
      i++;
  });
  res.send({
    arr,
    names,
    size
  });
  
});

app.listen(portNum);
console.log('Running app at localhost: ' + portNum);