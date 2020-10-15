const installBtn = document.getElementById('installbtn')
const title = document.getElementById('title')

installBtn.addEventListener('click', ()=>{
    getJSON('https://api.github.com/repos/lockieluke/Zinc/tags', function (status, data) {
        if (status == 200) {
            installBtn.style.visibility = 'hidden'
            const dataLength = data.length;
            const dataLatest = data[dataLength - 1]
            const vername = dataLatest.name
            SendMessage('https://github.com/lockieluke/Zinc/releases/download/' + vername.toString() + '/Zinc-win32-x64.7z')
        } else {
            installBtn.innerText = "Download unavailable"
            installBtn.style.pointerEvents = 'none'
        }
    });
})

const getJSON = (url, callback) => {
    const xhr = new XMLHttpRequest();
    xhr.open('GET', url, true);
    xhr.responseType = 'json';
    xhr.onload = () => {
        var status = xhr.status;
        callback(parseInt(status), xhr.response);
    };
    xhr.send();
};

function DownloadComplete() {
    title.innerText = "Download Completed"
}