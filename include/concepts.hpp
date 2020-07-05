#pragma once

template<class Promise, class ReturnObject>
concept return_object_aware = requires(Promise promise, ReturnObject* r) {
    { promise.set_return_object(r) };
};
