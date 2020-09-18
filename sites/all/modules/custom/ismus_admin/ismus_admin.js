(function ($) {
// Store our function as a property of Drupal.behaviors.
    Drupal.behaviors.myModuleSecureLink = {
        attach: function (context, settings) {
            $('#spotify-search-results').find('a.button').click(function(event) {
                event.preventDefault();
                var button = event.currentTarget.id;
                var number = button.slice(-1);
                var string = '#spotify-data-' + number;
                $('#edit-field-spotify-id-und-0-value').val( ($(string)[0].innerHTML) );
                $('#edit-field-spotify-artist-id-und-0-value').val( ($(string)[0].innerHTML) );
                event.currentTarget.innerHTML = "ID Afritað";
            });
            $('#discogs-search-results').find('a.button').click(function(event) {
                event.preventDefault();
                var button = event.currentTarget.id;
                var number = button.slice(-1);
                var string = '#discogs-data-' + number;
                console.log(string);
                $('#edit-field-discogs-id-und-0-value').val( ($(string)[0].innerHTML) );
                $('#edit-field-discogs-artist-id-und-0-value').val( ($(string)[0].innerHTML) );
                event.currentTarget.innerHTML = "ID Afritað";
            });
            $('#musicbrainz-search-results').find('a.button').click(function(event) {
                event.preventDefault();
                var button = event.currentTarget.id;
                var number = button.slice(-1);
                var string = '#musicbrainz-data-' + number;
                console.log(string);
                $('#edit-field-musicbrainz-id-und-0-value').val( ($(string)[0].innerHTML) );
                $('#edit-field-musicbrainzgi-artist-id-und-0-value').val( ($(string)[0].innerHTML) );
                event.currentTarget.innerHTML = "ID Afritað";
            });
        }
    };
}(jQuery));