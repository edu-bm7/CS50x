
document.addEventListener('DOMContentLoaded', function() {
    let btn1El = document.querySelectorAll('.btn-1');
    let correctBtn = document.getElementById('correct');
    let btnP = document.getElementById('btn-p');

    correctBtn.addEventListener('click', function() {
            correctBtn.style.backgroundColor = 'lightgreen';
            btnP.innerHTML = 'Correct!';
        });
    for (let n = 0; n<btn1El.length; n++){
        btn1El[n].addEventListener('click', function() {
            btn1El[n].style.backgroundColor = '#ff7f7f';
            btnP.innerHTML = 'Incorrect';

        });
    }

    let inputEl = document.querySelector('input');
    let inputP = document.querySelector('#input-p');
    let btnInput = document.getElementById('input-btn')

    btnInput.addEventListener('click', function (event) {
        event.preventDefault();
        let answer = 'Uruguay';
        if (inputEl.value.toLowerCase() === answer.toLowerCase()) {
            inputEl.style.backgroundColor = 'lightgreen';
            inputP.textContent = 'Correct!';
        } else {
            inputEl.style.backgroundColor = '#ff7f7f';
            inputP.textContent = 'Incorrect';
        }
    });
});



