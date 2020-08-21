// Put all onload AJAX calls here, and event listeners
$(document).ready(function() {
    // On page-load AJAX Example
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
    $("#upload-file").val('')

    init();

    



    

     // added effects to the header bar on scroll
     $(Window).scroll(function () {
        var start = $(window).scrollTop();
        if(start >= 60) {
            $("header").addClass('secondary');
        }
        else { 
            if($("header").hasClass('secondary')) {
                $("header").removeClass('secondary')
            }
        }
        
    });

    // Event listener form example , we can use this instead explicitly listening for events
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




function convertBytes(bytes) {
    var sizes = ['Bytes', 'KB', 'MB', 'GB', 'TB'];
    if(bytes == 0) return '0 KB';
    var i = parseInt(Math.floor(Math.log(bytes) / Math.log(1024)));
    if (i == 0) return bytes + ' ' + sizes[i];
    return (bytes / Math.pow(1024, i)).toFixed(1) + ' ' + sizes[i];
}



function init() {
    $.ajax({
        type: 'get',
        dataType: 'json',
        url: '/uploads',
        success: function(data) {
            $("#table-body").empty();
            $("#dropdown").empty();
            var table_body = document.getElementById('table-body');
            var drop_down = document.getElementById('drop-down');
            let scale_dropdown = document.getElementById('scale-dropdown');
            let add_shape_dropdown = document.getElementById('add_shape_dropdown');

            if(data.files.length == 0) {
                var row = ` <tr>
                                <h2>No Files</h2>
                            </tr>`
                table_body.innerHTML += row
                $("#table-body").addClass('table-css');
            }else {
                for(var i = 0; i < data.files.length; i++) {
                    let size = convertBytes(data.stats[i].size);
                    var row =   `<td>
                                    <a download="../uploads/${data.files[i]}" href="../uploads/${data.files[i]}">
                                    <img src="${data.files[i]}" id="image">
                                    </a>
                                </td>
                                <td>
                                    <a download="../uploads/${data.files[i]}" href="../uploads/${data.files[i]}">${data.files[i]}</a>
                                </td>
                                <td>
                                    <p>${size}</p>
                                </td>
                                <td>
                                    <p>${data.data[i].numRect}</p>
                                </td>
                                <td>
                                    <p>${data.data[i].numCirc}</p>
                                </td>
                                <td>
                                    <p>${data.data[i].numPaths}</p>
                                </td>
                                <td>
                                    <p>${data.data[i].numGroups}</p>
                                </td>
                                `
                    table_body.innerHTML += row

                    var content =   `<option value="${data.files[i]}">${data.files[i]}</option>
                    `
                    drop_down.innerHTML += content
                    scale_dropdown.innerHTML += content
                    add_shape_dropdown.innerHTML += content
                }
                $("#table-body").addClass('table-css');
                $("#image").addClass('image')
                if(data.files.length > 5) {
                    console.log(data.files.length)
                    $("#table-body").addClass('scroll-table')
                }
            }
        },
        fail: function(e) {
            console.log(e);
        }
    });
}




function showAttributes(index, shape) {
    let selectedBox = document.getElementById("drop-down")
    let selectedItem = selectedBox.options[selectedBox.selectedIndex].value

    $.ajax({
        type: 'get',
        dataType: 'json',
        url: '/getOtherAttributes',
        data: {
            fileName: selectedItem,
            i: index,
            s: shape
        },
        success: (data) => {
            alert('Other Attributes: ' + JSON.stringify(data.value[0]))
        },
        fail: (e) => {
            console.log(e)
        }
    });
}

function editTitle(string) {
    let value;
    let selectedBox = document.getElementById("drop-down")
    let selectedItem = selectedBox.options[selectedBox.selectedIndex].value

    if(string == 'title') {
        showTitle();
        $('#edit-title').on('click', () => {
            hideTitle();
            value = $("#title-form-value").val();

            $.ajax({
                type: 'get',
                dataType: 'json',
                url: '/editTitle',
                data: {
                    file: selectedItem,
                    name: string,
                    val: value 
                },
                success: (data) => {
                    console.log(data);
                    location.reload();
                },
                fail: (e) => {
                    console.log(e)
                }
            });

            return false;
        });
    }else if(string == 'desc') {
        showDescription();
        $('#edit-desc').on('click', () => {
            hideDescription();
            value = $("#desc-form-value").val();
            
            $.ajax({
                type: 'get',
                dataType: 'json',
                url: '/editTitle',
                data: {
                    file: selectedItem,
                    name: string,
                    val: value 
                },
                success: (data) => {
                    console.log(data);
                    location.reload();
                },
                fail: (e) => {
                    console.log(e)
                }
            });

            return false;
        });
    }

}




function editAttribute(index, shape) {
    let selectedBox = document.getElementById("drop-down")
    let selectedItem = selectedBox.options[selectedBox.selectedIndex].value

    showForm();
    $("#edit-btn").on("click", () => {
        hideForm();
        let name = $("#attribute-form-name").val();
        let value = $("#attribute-form-value").val();
        name = name.toLowerCase();
        value = value.toLowerCase();

        $.ajax({
            type: 'get',
            dataType: 'json',
            url: '/editAttribute',
            data: {
                fileName: selectedItem,
                i: index,
                s: shape,
                n: name,
                v: value
            },
            success: (data) => {
                console.log(data)
                location.reload()
            },
            fail: (e) => {
                console.log(e)
            }
        })
        return false;
    });
}

function showForm() {
    document.getElementById("form-popup").style.display = "block";
}

function hideForm() {
    document.getElementById("form-popup").style.display = "none";
}

function hideTitle() {
    document.getElementById("title-popup").style.display = "none";
}

function showTitle() {
    document.getElementById("title-popup").style.display = "block";
}

function hideDescription() {
    document.getElementById("desc-popup").style.display = "none";
}

function showDescription() {
    document.getElementById("desc-popup").style.display = "block";
}

function change() {
    var selectedBox = document.getElementById("drop-down");
    var selectedItem = selectedBox.options[selectedBox.selectedIndex].value
    $.ajax({
        type: 'get',
        dataType: 'json',
        url: '/upload/'+ selectedItem,
        success: function(data) {
            var image = document.getElementById('view-panel-image')
            var imageRow =  `<td colspan="3" scope="colgroup" style="padding-left: 100px; padding-right: 100px;">
                                <img src="${selectedItem}" colspan="3" scope="colgroup">
                            </td>
            `
            image.innerHTML = imageRow

            var tRow = document.getElementById('title-row')
            var titleRow = `<td colspan="1" scope="colgroup">
                                <p>${data.data.title}</p>
                                <button class="first" onclick="editTitle('title')">Edit Title</button>
                            </td>
                            <td colspan="2" scope="colgroup">
                                <p>${data.data.desc}</p>
                                <button class="first" onclick="editTitle('desc')">Edit Description</button>
                            </td>
            `
            tRow.innerHTML = titleRow
            let svgBody = document.getElementById('view-panel-body')
            let i;
            let body = ``;

            for(i = 0; i < data.data.rects.length; i++) {
                body =     `<td>
                                <p>Rectangle ${i + 1}</p>
                            </td>
                            <td>
                                <p>Upper left corner: x = ${data.data.rects[i].x}${data.data.rects[i].units}, y = ${data.data.rects[i].y}${data.data.rects[i].units}, Width: ${data.data.rects[i].w}${data.data.rects[i].units}, Height: ${data.data.rects[i].h}${data.data.rects[i].units}</p>
                                <button class="first" value="${i}" onclick="editAttribute(this.value, 'Rectangle')">Add Attribute</button>
                            </td>
                            <td>
                                <p>${data.data.rects[i].numAttr}</p>
                                <div style="display: inline-block;">
                                    <button class="first" value="${i}" onclick="showAttributes(this.value, 'rect')">Show Attributes</button>
                                    <button class="first" value="${i}" onclick="editAttribute(this.value, 'Rectangle')">Edit Attributes</button>
                                </div>
                            </td>
                `
                svgBody.innerHTML += body

            }
            for(i = 0; i < data.data.circles.length; i++) {
                body =     `<td>
                                <p>Circle ${i + 1}</p>
                            </td>
                            <td>
                                <p>Centre: x = ${data.data.circles[i].cx}${data.data.circles[i].units}, y = ${data.data.circles[i].cy}${data.data.circles[i].units}, radius = ${data.data.circles[i].r}${data.data.circles[i].units}</p>
                                <button class="first" value="${i}" onclick="editAttribute(this.value, 'Circle')">Add Attribute</button>
                            </td>
                            <td>
                                <p>${data.data.circles[i].numAttr}</p>
                                <div style="display: inline-block;">
                                    <button class="first" value="${i}" onclick="showAttributes(this.value, 'circle')">Show Attributes</button>
                                    <button class="first" value="${i}" onclick="editAttribute(this.value, 'Circle')">Edit Attributes</button>
                                </div>
                            </td>
                `
                svgBody.innerHTML += body
            }
            for(i = 0; i < data.data.paths.length; i++) {
                body =     `<td>
                                <p>Path ${i + 1}</p>
                            </td>
                            <td>
                                <p>path data = ${data.data.paths[i].d}</p>
                                <button class="first" value="${i}" onclick="editAttribute(this.value, 'Path')">Add Atrribute</button>
                            </td>
                            <td>
                                <p>${data.data.paths[i].numAttr}</p>
                                <div style="display: inline-block;">
                                    <button class="first" value="${i}" onclick="showAttributes(this.value, 'path')">Show Attributes</button>
                                    <button class="first" value="${i}" onclick="editAttribute(this.value, 'Path')">Edit Attributes</button>
                                </div>
                            </td>
                `
                svgBody.innerHTML += body
            }
            for(i = 0; i < data.data.groups.length; i++) {
                body =     `<td>
                                <p>Group ${i + 1}</p>
                            </td>
                            <td>
                                <p>path data = ${data.data.groups[i].children} child elements</p>
                                <button class="first" value="${i}" onclick="editAttribute(this.value, 'Group')">Add Attribute</button>
                            </td>
                            <td>
                                <p>${data.data.groups[i].numAttr}</p>
                                <div style="display: inline-block;">
                                    <button class="first" value="${i}" onclick="showAttributes(this.value, 'group')">Show Attributes</button>
                                    <button class="first" value="${i}" onclick="editAttribute(this.value, 'Group')">Edit Attributes</button>
                                </div>
                            </td>
                `
                svgBody.innerHTML += body
            }
            $("#view-panel-body").addClass('table-css');
        },
        fail: function(e) {
            console.log(e)
        }
    });
}


function showBlock() {
    var selectedBox = document.getElementById("c-shape");
    var selectedItem = selectedBox.options[selectedBox.selectedIndex].value

    if(selectedItem == 'rect') {
        document.getElementById("rect-block").style.display = "inline-block";
        document.getElementById("circle-block").style.display = "none";
        $('#cx').val('')
        $("#cy").val('')
        $("#r").val('')
        $("sel").val('')
    }else if(selectedItem == 'circle') {
        document.getElementById("circle-block").style.display = "inline-block";
        document.getElementById("rect-block").style.display = "none";
        $("#x").val('')
        $("#y").val('')
        $("#width").val('')
        $("#height").val('')
        $("uni").val('')

    }else {
        document.getElementById("rect-block").style.display = "none";
        document.getElementById("circle-block").style.display = "none";

    }
}