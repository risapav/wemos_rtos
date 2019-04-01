"use strict";

//import * as $ from '/js/jquery-3.3.1.min.js';
//import * as _ from '/js/underscore-min.js';
import '/js/jquery-3.3.1.min.js.gz';
import '/js/underscore-min.js';
import '/js/bootstrap.min.js';

var self = this;

let objekty = {};

function startTime() {
  $.getJSON( "/fbk", { name: "John", time: "2pm" } )
    .done(function( json ) {
      $.each( json, function( key, val ) {
        console.log(key,val,objekty[key]);
        if(objekty.hasOwnProperty(key) && objekty[key] !== undefined){
          objekty[key].val( val );
        }
      });
    })
    .fail(function( jqxhr, textStatus, error ) {
      var err = textStatus + ", " + error;
      console.log( "Request Failed: " + err );
    });
  var t = setTimeout(startTime, 2000);
  console.log(objekty);
}

_.map(['time','c_garaz','c_svetlo','c_den_noc','c_rezerva','f_garaz','f_svetlo','f_den_noc','f_rezerva','f_temp1', 'f_temp2'],function(key, num){
  objekty[key] = $('[data-tab=\"' + key + '\"]');
});
startTime();
