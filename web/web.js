$(document).ready(function () {
  changeState("GridEntry"); //start here

  $('#map-sel-btn').click(function(){
    $('#map-sel-text,#map-text-done').toggle("fast", function(){
      if($('#map-sel-text').is(":hidden")){
        $('#map-sel-btn').text('Show text entry');
        $('#map-text-note').hide();
      }
      else{
        $('#map-sel-btn').text('Hide text entry');
        $('#map-text-note').show();
      }
    });
  });

  $('#map-text-done').click(function(){
    displayGrid($('#map-sel-text').val()+'\n');

    changeState("PopEntry");
  });

  //handle dropdowns
  $(".dropdown-menu li a").click(function(){
    $(this).parents(".btn-group").find('.selection').text($(this).text());
  });

  $('#map-drop-done').click(function(){
    var gridfname = $('#map-dropdown').children(":selected").attr('value');
    
    console.log(gridfname) //debug 
    
    if(gridfname!="" && gridfname!=undefined){ //if not default value

      $.ajax({
      	url: '/cgi-bin/covidsim_file.cgi?ftype=g&fname='+gridfname,
      	type: "get",
      	dataType: 'text',
      	success: displayGrid,
      	error: function(){alert("Error: Something went wrong");}
      });

      changeState("PopEntry");
    }
  });

  $('#ppl-sel-btn').click(function(){
    $('#ppl-entry').toggle("fast", function(){
      if($('#ppl-entry').is(":hidden")){
        $('#ppl-sel-btn').text('Start manual entry');
      }
      else{
        $('#ppl-sel-btn').text('Hide manual entry');
      }
    });
  });
  
  $('#add-p-btn').click(function(){
    let div = $('#ppl-list').children().last().clone();
    div.find('input:text').val('')
    $('#ppl-list').append('<hr>');
    $('#ppl-list').append(div);
  })

  $('#ppl-text-done').click(function(){
    changeState("TimeSteps");
    let ppl_text = parseEntries($('#ppl-entry'));
    displayPopGrid(ppl_text);
  });

  $('#ppl-drop-done').click(function(){
    var popfname = $('#ppl-dropdown').children(":selected").attr('value');

    if(popfname!="" && popfname!=undefined){ //if not default value
    $.ajax({
      url: '/cgi-bin/covidsim_file.cgi?ftype=p&fname='+popfname,
      type: "get",
      dataType: 'text',
      success: displayPopGrid,
      error: function(){alert("Error: Something went wrong");}
    });
    changeState("TimeSteps");
  }
  });

  $('#start').click(function(){       
    //get the grid dimensions
    let gx = 1+parseInt($('#grid-h').text().split(' ')[$('#grid-h').text().split(' ').length-1]);
    let gy = 1+parseInt($('#grid-v').text().split(' ')[$('#grid-v').text().split(' ').length-1]);
    
    let grid = $('#grid').html();
    grid = grid.replaceAll('<span class="x"></span>',"X");
    grid = grid.replaceAll('<span class="i"></span>',"_");
    grid = grid.replaceAll('<span class="s"></span>',"_");
    grid = grid.replaceAll('<span class="r"></span>',"_");
    grid = grid.replaceAll('<span> </span>', '_');
    grid = grid.replaceAll('\n', ''); //maybe??
    
    //get the population fname
    let ppl = $('#ppl-dropdown').children(":selected").attr('value');
    if(ppl == undefined || ppl==""){
      ppl = parseEntries($('#ppl-entry'));
      ppl = ppl.replaceAll(' ', '_');
      ppl = '&pf=0&pop=' + ppl;
    }
    else{
      ppl = '&pf=1&pop=' + ppl;
    }
    
    //get the timesteps 
    let ts = $('#ts').val();

    $.ajax({
      url: '/cgi-bin/covidsim_sim.cgi?xdim='+gx+'&ydim='+gy+'&grid='+grid+ppl+'&timesteps='+ts,
      type: "get",
      dataType: 'text',
      success: animateGrid,
      error: function(){alert("Error: something is wrong");}
    });
    
    changeState("Simulation");
  });

  $('#reset').click(function(){
    $('#map-dropdown').prop('selectedIndex', 0);
    $('#ppl-dropdown').prop('selectedIndex', 0);
    $('#map-sel-text').val('');
    $('#ts').val('');
    $('#grid').empty();
    $('#grid-h').empty();
    $('#grid-v').empty();
    changeState("GridEntry");
  });
  
  //NOTE: #rerun is currently commented out of the HTML
  $('#rerun').click(function(){
    changeState("Simulation");
    //TODO: more once sim is done
  });

});

function displayGrid(grid){
  $('#grid').empty();

  console.log(grid);//debug

  var lining = grid.split("\n"); grid="";
  let max=0;
  lining = lining.slice(0, -1);
  console.log(lining);//debug 
  lining.forEach(function(a,i){
    if(a.length > max){
      max = a.length;
    }
  });
  lining.forEach(function(a,i){
    while(a.length < max){
      a = a+' ';
    }
    grid+=a+'\n';
  });

  $('#map-sel-text').val(grid);
  console.log($('#map-sel-text').val());//debug

  var vert = "<br>";
  for(var i=0;i<lining.length;i++){
    vert += " " + i + "<br>";
  }
  var horiz = "";
  for(var i=0;i<max;i++){
    horiz += " " + i;
  }

  var text = grid.split(""); let endgrid="";
  text.forEach(function(char){
    if(char == ' ' || char == '_'){
      endgrid += '<span> </span>';
    }
    else if(char == '\n'){
      endgrid += char;
    }
    else{
      endgrid += '<span class="x">' + ' ' + '</span>';
      }
  });

  $('#grid-h').html(horiz);
  $('#grid-v').html(vert);
  $('#grid').html(endgrid);
}

function displayGridFrame(grid){
  $('#grid').empty();

  var text = grid.split(""); let endgrid="";
  text.forEach(function(char){
    if(char == ' ' || char == '_'){
      endgrid += '<span> </span>';
    }
    else if(char == '\n'){
      endgrid += char;
    }
    else if(char == "I"){
      endgrid += '<span class="i">' + '</span>';
    }
    else if(char == "S"){
      endgrid += '<span class="s">' + '</span>';
    }
    else if(char == "R"){
      endgrid += '<span class="r">' + '</span>';
    }
    else{
      endgrid += '<span class="x">' + '</span>';
      }
  });

  $('#grid').html(endgrid);
}

function parseEntries(ppl_entries){
  let p_arr = $('#ppl-entry').find('.p-sel').map(function() {
   let p_str = String('');
   p_str += $(this).find('.isr').val();
   if($(this).find('.vacc').checked) p_str = p_str + ' v'
   else p_str = p_str + ' u';
   p_str += ' ' + $(this).find('.posn').val().replace(',','');
   p_str += ' ' + $(this).find('.head').val().replace(',','');
   p_str += ' ' + $(this).find('.mask').children(":selected").attr('value');
   console.log(p_str);//debug
   return { p_str };
  }).get();
    
  let pt_arr = [];
  p_arr.forEach(function(o){
    pt_arr.push(o.p_str);
  })
  
  let p_text = pt_arr.join('/')+'/';
  
  return p_text;
}

function populate(grid, pop){
  console.log(pop); //debug
  pop = pop.split("/").slice(0, -1);
  console.log(pop);//debug
  var ppl = Array(grid.size).fill(' '); //this isn't working as intended
   //but the whole thing does what it's supposed to anyway, so...??
  var len = grid.split("\n")[0].length+1;

  pop.forEach(function(p){
    p = p.split(' ');
    ppl[parseInt(p[2])+len*parseInt(p[3])] = p[0];
  });
  
  arr = grid.split("");
  for(var i=0; i<arr.length; i++){
    if((ppl[i]=='R'||ppl[i]=='S'||ppl[i]=='I')&& arr[i]==' '){
      arr[i] = ppl[i].charAt(0);
    }
  }
  grid = "";
  arr.forEach(function(char){
    grid += char;
  });

  return grid;
}

function displayPopGrid(pop){
  $('#grid').empty();
  
  console.log(pop);//debug

  var grid = populate($('#map-sel-text').val(),pop);

  var text = grid.split(""); let endgrid="";
  text.forEach(function(char){
    if(char == '\n'){
      endgrid += char;
      }
    else if(char == ' '){
      endgrid+= '<span> </span>'
      }
    else if(char == "I"){
      endgrid += '<span class="i">' + '</span>';
    }
    else if(char == "S"){
      endgrid += '<span class="s">' + '</span>';
    }
    else if(char == "R"){
      endgrid += '<span class="r">' + '</span>';
    }
    else{
      endgrid += '<span class="x">' + '</span>';
      }
  });

  $('#grid').html(endgrid);
}

function animateGrid(gridList){
    gridList = gridList.split('=');
    console.log(gridList);//debug
  
    for(let i=0; i<gridList.length-1; i++){
    setTimeout(function() {
          displayGridFrame(gridList[i]);
        }, 1300*i);
      }
    
    $('#case-count-n').text(gridList[gridList.length-1]);
    
    setTimeout(
      function(){ changeState("SimEnd"); }, 1300*gridList.length
    );
}

function changeState(pageState){
$('.container').hide();
  switch(pageState){
    case "SimEnd":
    $('#fin-options').show();
    $('#case-count').show();
      break;
    case "Simulation":
    $('#timesteps').hide();
      break;
    case "TimeSteps":
    $('textarea').hide();
    $('#ppl-sidebar').hide();
    $('#timesteps').show();
      break;
    case "PopEntry":
    $('#ppl-sel-btn').text('Start manual entry');
    $('textarea').hide();
    $('#grid').show();
    $('#grid-entry').hide();
    $('#ppl-sidebar').show();
    $('#ppl-entry').hide();
      break;
    case "GridEntry":
    $('textarea').hide();
    $('#grid').hide();
    $('#grid-entry').show();
    $('#map-text-done').hide();
    $('#map-text-note').hide();
    $('#ppl-sidebar').hide();
    $('#timesteps').hide();
    $('#fin-options').hide();
    $('#case-count').hide();
      break;
  }
}
