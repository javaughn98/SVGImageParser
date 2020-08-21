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

 console.log(uploadFile)
  // Use the mv() method to place the file somewhere on your server
  if(fs.existsSync('uploads/' + uploadFile.name)) {
    return res.status(400).send(`This file ${uploadFile.name} already exists in uploads folder`)
  }else {
    uploadFile.mv('uploads/' + uploadFile.name, function(err) {
      if(err) {
        return res.status(500).send(err);
      }

      res.redirect('/');
    });
  }
});

//Respond to GET requests for files in the uploads/ directory
app.get('/uploads/:name', function(req , res){
  fs.stat('uploads/' + req.params.name, function(err, stat) {
    if(err == null) {
      console.log("no error sending files now");
      res.sendFile(path.join(__dirname+'/uploads/' + req.params.name));
    } else {
      console.log('Error in file downloading route: '+err);
      res.send('Cannot get files');
    }
  });
});

//******************** Your code goes here ******************** 

//Sample endpoint

let shared = ffi.Library('./libsvgparse', {
  'JsSVG' : ['string', ['string']],
  'vaildateJavascript' : ['bool', ['string']],
  'createSVG' : ['string', ['string']],
  'showOtherAttributes' : ['string', ['string', 'string', 'int']],
  'editAttribute' : ['void', ['string', 'string', 'int', 'string', 'string']],
  'editTitle': ['bool', ['string', 'string', 'string']],
  'upload' : ['int', ['string', 'string']],
  'scale' : ['bool', ['string', 'string', 'int']],
  'addShape' : [ 'bool', ['string', 'string', 'string']],
  'create' : ['bool', ['string', 'string']] 
});

app.get('/someendpoint', function(req, res){
  let retStr = req.query.name1 + " " + req.query.name2;
  res.send({
    foo: retStr
  });
});

app.get('/upload/:filename', function(req, res) {
  var svgString = shared.createSVG(req.params.filename);
  if(svgString == null) {
    console.log('Selected image is invalid');
  } else {
    var string = JSON.parse(svgString)
    res.send({
      data: string
    });
  }
});


app.get('/uploads', function(req, res) {
  fs.readdir(path.join(__dirname + '/uploads'), function(e, images) {
    if(e == null) {
      var Stat = [];
      var fileStat;
      var svgData = [];
      for(var i = 0; i < images.length; i++){
        if(shared.vaildateJavascript(images[i]) == true) {
          let returnString = shared.JsSVG(images[i]);
          svgData.push(JSON.parse(returnString));
          fileStat = fs.statSync(path.join(__dirname + `/uploads/${images[i]}`));
          Stat.push(fileStat);
        }else {
          console.log("file is invalid");
        }
      };

      res.send({
        files: images,
        stats: Stat,
        data: svgData
      });
    }else {
      console.log('Error in file downloading route: '+err);
      res.send('Cannot get files');
    }
  })
});

app.get('/getOtherAttributes', (req, res) => {
  let string = shared.showOtherAttributes(req.query.fileName, req.query.s, req.query.i);

  if(string == null) {
    console.log('error associated with the selected attributes');
    res.status(400).send({
      "error": "null attribute value was returned"
    });
  } else {
    let data = [];
    data.push(JSON.parse(string))
    res.status(200).send({
      value: data
    });
  }
});


app.get('/editAttribute', (req, res) => {
  let fileName = req.query.fileName
  let index = req.query.i
  let shape = req.query.s
  let name = req.query.n
  let value = req.query.v
  console.log(name)
  console.log(value)

  shared.editAttribute(fileName, shape, index, name, value);
  res.status(200).send({'success': "file written"});

});

app.get('/editTitle', (req, res) => {
  let string = req.query.name
  let value = req.query.val
  let fileName = req.query.file

  let result = shared.editTitle(fileName, string, value);
  if(result == false) {
    res.status(400).send({
      "error": "File was not written!"
    });
  }else {
    res.status(200).send({
      "success": "file was written successfully"
    });
  }

});

app.get('/uploadFile', (req, res) => {
  let fileName = req.query.fileName
  let newFile = req.query.newFile

  let val = shared.upload(fileName, newFile);
  console.log(val)

  if(val == 0) {
    res.stats(200).send({
      name: newFile,
      success : {"success": "the file was successfully uploaded"},
      val : 0
    })
  }else if(val == 1) {
    res.send({
      name: newFile,
      warn: {"warning": "File was not written because it already exists"},
      val : 1
    })
  }else if(val == 2) {
    res.status(400).send({
      "error" : "File upload was unsuccessful due to errers"
    })
  }

});


app.post('/scaling', (req, res) => {
  let result = shared.scale(req.body.fileName, req.body.shape, req.body.scale)

  if(result == false) {
    return res.status(400).send('Scaling for this file was unsuccessfull and result was not written!')
  }

  res.status(200).redirect('/')

});

app.post('/addShape', (req, res) => {
  let jsonString;
  console.log(req)
  if(req.body.shape == 'rect') {
    jsonString = {
      "x": parseInt(req.body.x),
      "y": parseInt(req.body.y),
      "w": parseInt(req.body.width),
      "h": parseInt(req.body.height),
      "units": req.body.units
    }
  }else if(req.body.shape == 'circle') {
    jsonString = {
      "cx": parseInt(req.body.cx),
      "cy": parseInt(req.body.cy),
      "r": parseInt(req.body.r),
      "units": req.body.units
    }
  }

  let string = JSON.stringify(jsonString);
  console.log(string)
  let result = shared.addShape(req.body.fileName, req.body.shape, string);

  if(result == false) {
    return res.status(400).send(`error occured when trying to write ${req.body.fileName}. Please try again`);
  }

  res.status(200).redirect('/')

});

app.post('/createSvg', (req, res) => {
  let jsonString  = {
    "title": req.body.title,
    "descr": req.body.desc
  }

  let string = JSON.stringify(jsonString);
  console.log(string)
  console.log(req.body.imageName)
  if(fs.existsSync('uploads/' + req.body.imageName)) {
    return res.status(400).send(`This file ${req.body.imageName} already exists in uploads folder`)
  }

  let result = shared.create(req.body.imageName, string);
  if(result == false) {
    return res.status(400).send(`error occured when trying to create ${req.body.imageName}. Please try again`);
  }
  
  res.status(200).redirect('/')

});

app.listen(portNum);
console.log('Running app at localhost: ' + portNum);