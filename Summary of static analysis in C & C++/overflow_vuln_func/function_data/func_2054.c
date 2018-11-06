static void set_int32(Object *obj, Visitor *v, void *opaque,
                      const char *name, Error **errp)
{
    DeviceState *dev = DEVICE(obj);
    Property *prop = opaque;
    int32_t *ptr = qdev_get_prop_ptr(dev, prop);
    Error *local_err = NULL;
    int64_t value;

    if (dev->state != DEV_STATE_CREATED) {
        error_set(errp, QERR_PERMISSION_DENIED);
        return;
    }

    visit_type_int(v, &value, name, &local_err);
    if (local_err) {
        error_propagate(errp, local_err);
        return;
    }
    if (value > prop->info->min && value <= prop->info->max) {
        *ptr = value;
    } else {
        error_set(errp, QERR_PROPERTY_VALUE_OUT_OF_RANGE,
                  dev->id?:"", name, value, prop->info->min,
                  prop->info->max);
    }
}
