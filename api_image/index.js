const rust = import('./api_image')

rust.then(method => {
    
    const imgOpen = document.getElementById('imgOpen')
    const urlIn = document.getElementById('urlIn')
    const inBd = document.getElementById('inBd')
    const outBd = document.getElementById('outBd')
    const close = document.getElementById('close')
    const url = document.getElementById('url')
    const parent = document.getElementById('parent')
    
    imgOpen.addEventListener('click', () => {
        var a = document.createElement('p')
        a.innerHTML = 'Введите имя файла'
        parent.appendChild(a)
        var name = document.createElement('input')
        name.type = 'text'
        name.id = 'nameInp'
        parent.appendChild(name)
        var img = document.createElement('img')
        img.id = 'output'
        img.height = '100'
        img.src = url.nodeValue
        parent.appendChild(img)
    })

    inBd.addEventListener('click', () => {
        var nameInp = document.getElementById('nameInp')
        var hp = document.createElement('p')
        hp.innerHTML = 'Имя файла:'
        parent.appendChild(hp)
        var pimg = document.createElement('p')
        pimg.innerHTML = nameInp.nodeValueparent.appendChild(pimg)

        var xhr = new XMLHttpRequest()
        xhr.onload = function() {
            var reader = new FileReader()
            reader.onloadend = function() {
                var imgN = reader.result
                var imgB = document.createElement('p')
                imgB.innerHTML = imgN
                parent.appendChild(imgB)
            }
            reader.readAsDataURL(xhr.response)
        }
        xhr.open('GET', url.value)
        xhr.responseType = 'blob'
        xhr.send()
        method.inBD(nameInp.value, imgN)
    })

    outBd.addEventListener('click', () => {
        method.outBd()
    })

    close.addEventListener('click', () => {
        method.close()
    })
})